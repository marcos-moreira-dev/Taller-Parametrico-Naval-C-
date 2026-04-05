#include "ScenarioSaver.hpp"

#include <fstream>
#include <iomanip>
#include <sstream>

namespace tp::persistence {

namespace {

char cellToChar(tp::shared::CellType cell) {
    switch (cell) {
        case tp::shared::CellType::Land: return 'L';
        case tp::shared::CellType::Obstacle: return 'O';
        case tp::shared::CellType::Water:
        default: return '.';
    }
}

std::string escapeValue(const std::string& value) {
    std::string escaped;
    escaped.reserve(value.size());
    for (char ch : value) {
        if (ch == '\n') {
            escaped += "\\n";
        } else if (ch != '\r') {
            escaped += ch;
        }
    }
    return escaped;
}

} // namespace

bool ScenarioSaver::saveToFile(const ScenarioFileData& data, const std::string& filepath) {
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    file << saveToJson(data);
    return file.good();
}

std::string ScenarioSaver::saveToJson(const ScenarioFileData& data) {
    std::ostringstream output;
    output << std::fixed << std::setprecision(6);

    const auto& scenario = data.scenario;
    const auto position = data.boat.getPosition();
    output << "# Taller Parametrico scenario file\n";
    output << "version=1\n";
    output << "name=" << escapeValue(scenario.getName()) << "\n";
    output << "width=" << scenario.getWidth() << "\n";
    output << "height=" << scenario.getHeight() << "\n";
    output << "boat_x=" << position.x << "\n";
    output << "boat_y=" << position.y << "\n";
    output << "boat_angle_rad=" << data.boat.getOrientation() << "\n";
    output << "field_type=" << data.fieldType << "\n";
    output << "field_intensity=" << data.fieldIntensity << "\n";
    output << "field_center_x=" << data.fieldCenterX << "\n";
    output << "field_center_y=" << data.fieldCenterY << "\n";
    output << "field_fx=" << escapeValue(data.fieldFx) << "\n";
    output << "field_fy=" << escapeValue(data.fieldFy) << "\n";
    output << "time_step=" << data.timeStep << "\n";
    output << "max_time=" << data.maxTime << "\n";
    output << "integration_method=" << data.integrationMethod << "\n";
    output << "[grid]\n";
    for (int y = 0; y < scenario.getHeight(); ++y) {
        for (int x = 0; x < scenario.getWidth(); ++x) {
            output << cellToChar(scenario.getCell(x, y));
        }
        output << "\n";
    }
    output << "[/grid]\n";
    return output.str();
}

} // namespace tp::persistence
