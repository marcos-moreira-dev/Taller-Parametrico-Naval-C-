#pragma once

/**
 * @file ScenarioLoader.hpp
 * @brief Carga de escenarios desde archivos
 * @author Proyecto Taller Paramétrico
 * @version 0.2.3
 * 
 * Carga escenarios desde:
 * - Archivos de disco (loadFromFile)
 * - JSON strings (loadFromJson)
 * 
 * @see ScenarioSaver para guardado
 * @see Scenario para formato interno
 */

#include "Scenario.hpp"
#include <string>
#include <optional>

namespace tp::persistence {

using namespace tp::domain;

class ScenarioLoader {
public:
    static std::optional<Scenario> loadFromFile(const std::string& filepath);
    static std::optional<Scenario> loadFromJson(const std::string& json);
};

}
