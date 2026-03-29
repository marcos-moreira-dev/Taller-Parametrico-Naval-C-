#include "ScenarioLoader.hpp"
#include <fstream>
#include <sstream>

namespace tp::persistence {

std::optional<Scenario> ScenarioLoader::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return std::nullopt;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    return loadFromJson(buffer.str());
}

std::optional<Scenario> ScenarioLoader::loadFromJson(const std::string& json) {
    (void)json;
    return Scenario(50, 50);
}

}
