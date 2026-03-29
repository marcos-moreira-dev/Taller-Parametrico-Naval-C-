#pragma once

/**
 * @file UserPreferences.hpp
 * @brief Sistema de preferencias de usuario
 * @author Proyecto Taller Paramétrico
 * @version 0.2.4
 * 
 * Gestiona la carga y guardado de preferencias de usuario:
 * - Configuración de ventana (tamaño, posición)
 * - Preferencias de simulación (dt por defecto, método)
 * - Preferencias de UI (paneles visibles, theme)
 * - Historial de archivos abiertos
 * 
 * Formato: Archivo INI en directorio del usuario
 */

#include <string>
#include <vector>
#include <map>

namespace tp::application {

/**
 * @brief Administrador de preferencias de usuario
 * 
 * Implementa el patrón Singleton para acceso global.
 * Las preferencias se guardan automáticamente al cerrar.
 */
class UserPreferences {
public:
    static UserPreferences& getInstance();
    
    void load();
    void save();
    
    // Ventana
    std::string getLastWindowSize() const { return windowSize_; }
    void setLastWindowSize(const std::string& size) { windowSize_ = size; }
    
    std::string getLastWindowPosition() const { return windowPosition_; }
    void setLastWindowPosition(const std::string& pos) { windowPosition_ = pos; }
    
    bool getLeftPanelVisible() const { return leftPanelVisible_; }
    void setLeftPanelVisible(bool visible) { leftPanelVisible_ = visible; }
    
    bool getRightPanelVisible() const { return rightPanelVisible_; }
    void setRightPanelVisible(bool visible) { rightPanelVisible_ = visible; }
    
    bool getBottomPanelVisible() const { return bottomPanelVisible_; }
    void setBottomPanelVisible(bool visible) { bottomPanelVisible_ = visible; }
    
    // Simulación
    double getDefaultTimeStep() const { return defaultTimeStep_; }
    void setDefaultTimeStep(double dt) { defaultTimeStep_ = dt; }
    
    int getDefaultMethod() const { return defaultMethod_; }
    void setDefaultMethod(int method) { defaultMethod_ = method; }
    
    bool getShowGrid() const { return showGrid_; }
    void setShowGrid(bool show) { showGrid_ = show; }
    
    bool getShowField() const { return showField_; }
    void setShowField(bool show) { showField_ = show; }
    
    bool getShowTrajectory() const { return showTrajectory_; }
    void setShowTrajectory(bool show) { showTrajectory_ = show; }
    
    // Editor
    int getDefaultBrushSize() const { return defaultBrushSize_; }
    void setDefaultBrushSize(int size) { defaultBrushSize_ = size; }
    
    bool getSnapToGrid() const { return snapToGrid_; }
    void setSnapToGrid(bool snap) { snapToGrid_ = snap; }
    
    // Color scheme
    std::string getWaterColor() const { return waterColor_; }
    void setWaterColor(const std::string& color) { waterColor_ = color; }
    
    std::string getLandColor() const { return landColor_; }
    void setLandColor(const std::string& color) { landColor_ = color; }
    
    std::string getObstacleColor() const { return obstacleColor_; }
    void setObstacleColor(const std::string& color) { obstacleColor_ = color; }
    
    // Archivos recientes
    void addRecentFile(const std::string& filePath);
    std::string getRecentFile(int index) const;
    int getRecentFileCount() const;
    
    // Preferences
    void setPreference(const std::string& key, const std::string& value);
    std::string getPreference(const std::string& key, const std::string& defaultValue = "") const;

private:
    UserPreferences();
    ~UserPreferences() = default;
    UserPreferences(const UserPreferences&) = delete;
    UserPreferences& operator=(const UserPreferences&) = delete;
    
    std::string getConfigPath() const;
    
    // Window settings
    std::string windowSize_;
    std::string windowPosition_;
    bool leftPanelVisible_;
    bool rightPanelVisible_;
    bool bottomPanelVisible_;
    
    // Simulation defaults
    double defaultTimeStep_;
    int defaultMethod_;
    bool showGrid_;
    bool showField_;
    bool showTrajectory_;
    
    // Editor defaults
    int defaultBrushSize_;
    bool snapToGrid_;
    
    // Colors
    std::string waterColor_;
    std::string landColor_;
    std::string obstacleColor_;
    
    // Recent files
    std::vector<std::string> recentFiles_;
    
    // Custom preferences
    std::map<std::string, std::string> customPrefs_;
};

} // namespace tp::application