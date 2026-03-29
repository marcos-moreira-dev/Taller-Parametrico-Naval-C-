// UserPreferences.cpp - Implementación del sistema de preferencias

#include "UserPreferences.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif

namespace tp::application {

UserPreferences& UserPreferences::getInstance() {
    static UserPreferences instance;
    return instance;
}

UserPreferences::UserPreferences()
    : windowSize_("1600,1000")
    , windowPosition_("-1,-1")
    , leftPanelVisible_(true)
    , rightPanelVisible_(true)
    , bottomPanelVisible_(true)
    , defaultTimeStep_(0.01)
    , defaultMethod_(0)  // Euler
    , showGrid_(true)
    , showField_(true)
    , showTrajectory_(true)
    , defaultBrushSize_(3)
    , snapToGrid_(false)
    , waterColor_("#4A90D9")
    , landColor_("#8B7355")
    , obstacleColor_("#555555")
{
}

std::string UserPreferences::getConfigPath() const {
#ifdef _WIN32
    char path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, path))) {
        return std::string(path) + "\\TallerParametrico.ini";
    }
    return "TallerParametrico.ini";
#else
    const char* home = getenv("HOME");
    if (!home) home = getpwuid(getuid())->pw_dir;
    return std::string(home) + "/.tallerparametrico";
#endif
}

void UserPreferences::load() {
    std::string configPath = getConfigPath();
    
    std::ifstream file(configPath);
    if (!file.is_open()) {
        return;
    }
    
    std::map<std::string, std::string> values;
    std::string line;
    while (std::getline(file, line)) {
        size_t eqPos = line.find('=');
        if (eqPos != std::string::npos && eqPos > 0) {
            std::string key = line.substr(0, eqPos);
            std::string value = line.substr(eqPos + 1);
            // Trim whitespace
            while (key.size() > 0 && (key[0] == ' ' || key[0] == '\t')) key.erase(0, 1);
            while (key.size() > 0 && (key.back() == ' ' || key.back() == '\t')) key.pop_back();
            while (value.size() > 0 && (value[0] == ' ' || value[0] == '\t')) value.erase(0, 1);
            while (value.size() > 0 && (value.back() == ' ' || value.back() == '\t')) value.pop_back();
            values[key] = value;
        }
    }
    file.close();
    
    // Window
    if (values.count("WindowSize")) windowSize_ = values["WindowSize"];
    if (values.count("WindowPosition")) windowPosition_ = values["WindowPosition"];
    if (values.count("LeftPanelVisible")) leftPanelVisible_ = values["LeftPanelVisible"] == "1";
    if (values.count("RightPanelVisible")) rightPanelVisible_ = values["RightPanelVisible"] == "1";
    if (values.count("BottomPanelVisible")) bottomPanelVisible_ = values["BottomPanelVisible"] == "1";
    
    // Simulation
    if (values.count("DefaultTimeStep")) defaultTimeStep_ = std::stod(values["DefaultTimeStep"]);
    if (values.count("DefaultMethod")) defaultMethod_ = std::stoi(values["DefaultMethod"]);
    if (values.count("ShowGrid")) showGrid_ = values["ShowGrid"] == "1";
    if (values.count("ShowField")) showField_ = values["ShowField"] == "1";
    if (values.count("ShowTrajectory")) showTrajectory_ = values["ShowTrajectory"] == "1";
    
    // Editor
    if (values.count("DefaultBrushSize")) defaultBrushSize_ = std::stoi(values["DefaultBrushSize"]);
    if (values.count("SnapToGrid")) snapToGrid_ = values["SnapToGrid"] == "1";
    
    // Colors
    if (values.count("WaterColor")) waterColor_ = values["WaterColor"];
    if (values.count("LandColor")) landColor_ = values["LandColor"];
    if (values.count("ObstacleColor")) obstacleColor_ = values["ObstacleColor"];
    
    // Recent files
    for (int i = 0; i < 10; i++) {
        std::string key = "RecentFile" + std::to_string(i);
        if (values.count(key)) {
            recentFiles_.push_back(values[key]);
        }
    }
    
    // Custom preferences
    for (const auto& pair : values) {
        if (pair.first.substr(0, 5) == "Pref_") {
            customPrefs_[pair.first.substr(5)] = pair.second;
        }
    }
}

void UserPreferences::save() {
    std::string configPath = getConfigPath();
    
    std::ofstream file(configPath);
    if (!file.is_open()) {
        return;
    }
    
    // Window
    file << "WindowSize=" << windowSize_ << "\n";
    file << "WindowPosition=" << windowPosition_ << "\n";
    file << "LeftPanelVisible=" << (leftPanelVisible_ ? "1" : "0") << "\n";
    file << "RightPanelVisible=" << (rightPanelVisible_ ? "1" : "0") << "\n";
    file << "BottomPanelVisible=" << (bottomPanelVisible_ ? "1" : "0") << "\n";
    
    // Simulation
    file << "DefaultTimeStep=" << defaultTimeStep_ << "\n";
    file << "DefaultMethod=" << defaultMethod_ << "\n";
    file << "ShowGrid=" << (showGrid_ ? "1" : "0") << "\n";
    file << "ShowField=" << (showField_ ? "1" : "0") << "\n";
    file << "ShowTrajectory=" << (showTrajectory_ ? "1" : "0") << "\n";
    
    // Editor
    file << "DefaultBrushSize=" << defaultBrushSize_ << "\n";
    file << "SnapToGrid=" << (snapToGrid_ ? "1" : "0") << "\n";
    
    // Colors
    file << "WaterColor=" << waterColor_ << "\n";
    file << "LandColor=" << landColor_ << "\n";
    file << "ObstacleColor=" << obstacleColor_ << "\n";
    
    // Recent files
    for (size_t i = 0; i < recentFiles_.size() && i < 10; i++) {
        file << "RecentFile" << i << "=" << recentFiles_[i] << "\n";
    }
    
    // Custom preferences
    for (const auto& pair : customPrefs_) {
        file << "Pref_" << pair.first << "=" << pair.second << "\n";
    }
    
    file.close();
}

void UserPreferences::addRecentFile(const std::string& filePath) {
    // Remove if exists
    for (auto it = recentFiles_.begin(); it != recentFiles_.end(); ++it) {
        if (*it == filePath) {
            recentFiles_.erase(it);
            break;
        }
    }
    
    // Add to front
    recentFiles_.insert(recentFiles_.begin(), filePath);
    
    // Keep only 10
    if (recentFiles_.size() > 10) {
        recentFiles_.resize(10);
    }
    
    save();
}

std::string UserPreferences::getRecentFile(int index) const {
    if (index >= 0 && index < (int)recentFiles_.size()) {
        return recentFiles_[index];
    }
    return "";
}

int UserPreferences::getRecentFileCount() const {
    return (int)recentFiles_.size();
}

void UserPreferences::setPreference(const std::string& key, const std::string& value) {
    customPrefs_[key] = value;
}

std::string UserPreferences::getPreference(const std::string& key, const std::string& defaultValue) const {
    auto it = customPrefs_.find(key);
    if (it != customPrefs_.end()) {
        return it->second;
    }
    return defaultValue;
}

} // namespace tp::application