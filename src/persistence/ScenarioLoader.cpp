#include "ScenarioLoader.hpp"

#include <fstream>
#include <sstream>

namespace tp::persistence {

namespace {

std::string trim(const std::string& value) {
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return {};
    }
    const auto last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

std::string unescapeValue(const std::string& value) {
    std::string result;
    result.reserve(value.size());
    for (size_t i = 0; i < value.size(); ++i) {
        if (value[i] == '\\' && (i + 1) < value.size() && value[i + 1] == 'n') {
            result += '\n';
            ++i;
        } else {
            result += value[i];
        }
    }
    return result;
}

tp::shared::CellType parseCell(char cell) {
    switch (cell) {
        case 'L': return tp::shared::CellType::Land;
        case 'O': return tp::shared::CellType::Obstacle;
        case '.':
        default: return tp::shared::CellType::Water;
    }
}

} // namespace

std::optional<ScenarioFileData> ScenarioLoader::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return std::nullopt;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return loadFromJson(buffer.str());
}

std::optional<ScenarioFileData> ScenarioLoader::loadFromJson(const std::string& json) {
    ScenarioFileData data;
    std::istringstream stream(json);
    std::string line;
    std::string scenarioName;
    bool inGrid = false;
    std::vector<std::string> gridLines;
    int width = 0;
    int height = 0;
    double boatX = 0.0;
    double boatY = 0.0;

    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        const std::string stripped = trim(line);
        if (stripped.empty() || stripped.rfind('#', 0) == 0) {
            continue;
        }
        if (stripped == "[grid]") {
            inGrid = true;
            continue;
        }
        if (stripped == "[/grid]") {
            inGrid = false;
            continue;
        }
        if (inGrid) {
            gridLines.push_back(stripped);
            continue;
        }

        const auto eq = stripped.find('=');
        if (eq == std::string::npos) {
            continue;
        }
        const std::string key = trim(stripped.substr(0, eq));
        const std::string value = trim(stripped.substr(eq + 1));

        if (key == "name") scenarioName = unescapeValue(value);
        else if (key == "width") width = std::stoi(value);
        else if (key == "height") height = std::stoi(value);
        else if (key == "boat_x") boatX = std::stod(value);
        else if (key == "boat_y") boatY = std::stod(value);
        else if (key == "boat_angle_rad") data.boat.setOrientation(std::stod(value));
        else if (key == "field_type") data.fieldType = std::stoi(value);
        else if (key == "field_intensity") data.fieldIntensity = std::stod(value);
        else if (key == "field_center_x") data.fieldCenterX = std::stod(value);
        else if (key == "field_center_y") data.fieldCenterY = std::stod(value);
        else if (key == "field_fx") data.fieldFx = unescapeValue(value);
        else if (key == "field_fy") data.fieldFy = unescapeValue(value);
        else if (key == "time_step") data.timeStep = std::stod(value);
        else if (key == "max_time") data.maxTime = std::stod(value);
        else if (key == "integration_method") data.integrationMethod = std::stoi(value);
    }

    if (width <= 0 || height <= 0 || static_cast<int>(gridLines.size()) != height) {
        return std::nullopt;
    }

    data.scenario = tp::domain::Scenario(width, height);
    data.scenario.setName(scenarioName);
    for (int y = 0; y < height; ++y) {
        if (static_cast<int>(gridLines[y].size()) != width) {
            return std::nullopt;
        }
        for (int x = 0; x < width; ++x) {
            data.scenario.setCell(x, y, parseCell(gridLines[y][x]));
        }
    }

    data.boat.reset();
    data.boat.setPosition({boatX, boatY});
    return data;
}

} // namespace tp::persistence
