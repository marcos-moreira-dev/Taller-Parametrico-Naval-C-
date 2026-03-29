#pragma once

/**
 * @file CSVExporter.hpp
 * @brief Exportador de datos a formato CSV
 * @author Proyecto Taller Paramétrico
 * @version 0.2.3
 * 
 * Exporta datos de simulación a formato CSV para:
 * - Importación en hojas de cálculo
 * - Análisis con herramientas estadísticas
 * - Integración con Python/R para plotting
 * 
 * @see JSONExporter para exportación JSON
 */

#include <string>
#include <vector>
#include <fstream>
#include "runner/SimulationRunner.hpp"

namespace tp::exporting {

class CSVExporter {
public:
    /**
     * @brief Exporta la trayectoria completa a formato CSV
     * @param filepath Ruta del archivo de salida
     * @param states Vector de estados físicos de la simulación
     * @return true si la exportación fue exitosa
     */
    static bool exportTrajectory(const std::string& filepath,
                                  const std::vector<tp::simulation::PhysicsState>& states);
    
    static bool exportSummary(const std::string& filepath,
                               const tp::simulation::SimulationResult& result);
    
    static std::string generateFilename(const std::string& prefix = "simulation");
};

}
