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
#include "Boat.hpp"
#include "Enums.hpp"
#include <string>

namespace tp::persistence {

struct ScenarioFileData {
    tp::domain::Scenario scenario;
    tp::domain::Boat boat;
    int fieldType = 0;
    double fieldIntensity = 0.0;
    double fieldCenterX = 25.0;
    double fieldCenterY = 25.0;
    std::string fieldFx = "0";
    std::string fieldFy = "0";
    double timeStep = 0.01;
    double maxTime = 10.0;
    int integrationMethod = 0;

    ScenarioFileData()
        : scenario(50, 50) {}
};

class ScenarioSaver {
public:
    static bool saveToFile(const ScenarioFileData& data, const std::string& filepath);
    static std::string saveToJson(const ScenarioFileData& data);
};

}
