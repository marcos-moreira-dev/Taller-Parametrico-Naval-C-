#include "PresetFileRepository.hpp"

#include "MathExpression.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace tp::application {

namespace {

struct PresetFileData {
    std::string name;
    int width = 0;
    int height = 0;
    double boatX = 0.0;
    double boatY = 0.0;
    double boatAngleDeg = 0.0;
    FieldPresetType fieldType = FieldPresetType::Uniform;
    double fieldIntensity = 0.0;
    double fieldCenterX = 0.0;
    double fieldCenterY = 0.0;
    std::string fieldFx;
    std::string fieldFy;
    std::vector<std::string> grid;
};

std::string trim(const std::string& value) {
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return {};
    }
    const auto last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

std::string readUtf8File(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);
    if (!input) {
        throw std::runtime_error("No se pudo abrir preset: " + path.string());
    }
    std::ostringstream buffer;
    buffer << input.rdbuf();
    std::string data = buffer.str();
    if (data.size() >= 3 && static_cast<unsigned char>(data[0]) == 0xEF &&
        static_cast<unsigned char>(data[1]) == 0xBB && static_cast<unsigned char>(data[2]) == 0xBF) {
        data.erase(0, 3);
    }
    return data;
}

FieldPresetType parseFieldType(const std::string& value) {
    if (value == "uniform") return FieldPresetType::Uniform;
    if (value == "linear") return FieldPresetType::Linear;
    if (value == "radial") return FieldPresetType::Radial;
    if (value == "rotational") return FieldPresetType::Rotational;
    return FieldPresetType::Custom;
}

PresetFileData parsePresetFile(const std::filesystem::path& path) {
    PresetFileData data;
    std::istringstream stream(readUtf8File(path));
    std::string line;
    bool inGrid = false;

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
            data.grid.push_back(stripped);
            continue;
        }

        const auto eq = stripped.find('=');
        if (eq == std::string::npos) {
            continue;
        }

        const std::string key = trim(stripped.substr(0, eq));
        const std::string value = trim(stripped.substr(eq + 1));

        if (key == "name") data.name = value;
        else if (key == "width") data.width = std::stoi(value);
        else if (key == "height") data.height = std::stoi(value);
        else if (key == "boat_x") data.boatX = std::stod(value);
        else if (key == "boat_y") data.boatY = std::stod(value);
        else if (key == "boat_angle_deg") data.boatAngleDeg = std::stod(value);
        else if (key == "field_type") data.fieldType = parseFieldType(value);
        else if (key == "field_intensity") data.fieldIntensity = std::stod(value);
        else if (key == "field_center_x") data.fieldCenterX = std::stod(value);
        else if (key == "field_center_y") data.fieldCenterY = std::stod(value);
        else if (key == "field_fx") data.fieldFx = value;
        else if (key == "field_fy") data.fieldFy = value;
    }

    if (data.width <= 0 || data.height <= 0 || static_cast<int>(data.grid.size()) != data.height) {
        throw std::runtime_error("Preset inválido: dimensiones o grid inconsistentes en " + path.string());
    }

    return data;
}

ScenarioPreset buildPreset(const PresetFileData& data) {
    return ScenarioPreset{
        data.name,
        [data](ExperimentConfig& config) {
            config.scenario = Scenario(data.width, data.height);
            config.scenario.setName(data.name);
            for (int y = 0; y < data.height; ++y) {
                if (static_cast<int>(data.grid[y].size()) != data.width) {
                    throw std::runtime_error("Fila de grid con ancho inválido en preset: " + data.name);
                }
                for (int x = 0; x < data.width; ++x) {
                    switch (data.grid[y][x]) {
                        case 'L': config.scenario.setCell(x, y, CellType::Land); break;
                        case 'O': config.scenario.setCell(x, y, CellType::Obstacle); break;
                        case '.':
                        default: config.scenario.setCell(x, y, CellType::Water); break;
                    }
                }
            }

            config.boat.reset();
            config.boat.setPosition(Vec2d(data.boatX, data.boatY));
            config.boat.setOrientation(data.boatAngleDeg * 3.14159265358979323846 / 180.0);

            MathExpression parser;
            if (!parser.parse(data.fieldFx, data.fieldFy)) {
                throw std::runtime_error("No se pudo parsear el campo del preset: " + data.name);
            }
            config.field = VectorField([parser](double x, double y) {
                return parser.evaluate(x, y);
            });

            config.fieldView.type = data.fieldType;
            config.fieldView.intensity = data.fieldIntensity;
            config.fieldView.centerX = data.fieldCenterX;
            config.fieldView.centerY = data.fieldCenterY;
        }
    };
}

} // namespace

PresetFileRepository::PresetFileRepository(std::filesystem::path rootDirectory)
    : rootDirectory_(std::move(rootDirectory)) {
}

std::vector<ScenarioPreset> PresetFileRepository::loadPresets() const {
    std::vector<std::filesystem::path> files;
    for (const auto& entry : std::filesystem::directory_iterator(rootDirectory_)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            files.push_back(entry.path());
        }
    }

    std::sort(files.begin(), files.end());

    std::vector<ScenarioPreset> presets;
    for (const auto& file : files) {
        presets.push_back(buildPreset(parsePresetFile(file)));
    }
    return presets;
}

} // namespace tp::application
