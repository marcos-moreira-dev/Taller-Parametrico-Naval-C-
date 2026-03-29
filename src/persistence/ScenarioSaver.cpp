#include "ScenarioSaver.hpp"
#include <fstream>

namespace tp::persistence {

bool ScenarioSaver::saveToFile(const Scenario& scenario, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << saveToJson(scenario);
    return true;
}

std::string ScenarioSaver::saveToJson(const Scenario& scenario) {
    return "{\"width\":" + std::to_string(scenario.getWidth()) + 
           ",\"height\":" + std::to_string(scenario.getHeight()) + "}";
}

}
