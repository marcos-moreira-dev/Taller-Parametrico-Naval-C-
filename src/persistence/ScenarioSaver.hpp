#pragma once

/**
 * @file ScenarioSaver.hpp
 * @brief Guardado de escenarios a archivos
 * @author Proyecto Taller Paramétrico
 * @version 0.2.3
 * 
 * Guarda escenarios a:
 * - Archivos de disco (saveToFile)
 * - JSON strings (saveToJson)
 * 
 * @see ScenarioLoader para carga
 * @see Scenario para formato interno
 */

#include "Scenario.hpp"
#include <string>

namespace tp::persistence {

using namespace tp::domain;

class ScenarioSaver {
public:
    static bool saveToFile(const Scenario& scenario, const std::string& filepath);
    static std::string saveToJson(const Scenario& scenario);
};

}
