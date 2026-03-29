#pragma once

#include <string>
#include <vector>
#include "runner/SimulationRunner.hpp"

namespace tp::exporting {

/**
 * @brief Exportador de datos de simulación en formato JSON
 * 
 * Proporciona exportación estructurada de trayectorias y resultados
 * en formato JSON para integración con otras herramientas y análisis.
 */
class JSONExporter {
public:
    /**
     * @brief Exporta la trayectoria completa a formato JSON
     * 
     * Genera un archivo JSON con toda la información de la simulación:
     * metadatos, estados individuales y estadísticas resumidas.
     * 
     * @param filepath Ruta del archivo de salida (.json)
     * @param states Vector de estados físicos de la simulación
     * @param scenarioName Nombre del escenario (opcional)
     * @return true si la exportación fue exitosa
     * 
     * @code
     * auto result = runner.run();
     * JSONExporter::exportTrajectory("sim.json", result.states, "Rio");
     * @endcode
     */
    static bool exportTrajectory(const std::string& filepath,
                                  const std::vector<tp::simulation::PhysicsState>& states,
                                  const std::string& scenarioName = "");
    
    /**
     * @brief Exporta resumen de simulación a JSON
     * 
     * Incluye métricas agregadas sin los datos de trayectoria completos,
     * útil para análisis estadístico y comparaciones rápidas.
     * 
     * @param filepath Ruta del archivo de salida
     * @param result Resultado completo de la simulación
     * @return true si la exportación fue exitosa
     */
    static bool exportSummary(const std::string& filepath,
                               const tp::simulation::SimulationResult& result);
    
    /**
     * @brief Exporta configuración de escenario a JSON
     * 
     * Permite guardar la configuración de un escenario para compartir
     * o reutilizar en futuras simulaciones.
     * 
     * @param filepath Ruta del archivo de salida
     * @param scenario Escenario a exportar
     * @return true si la exportación fue exitosa
     */
    static bool exportScenario(const std::string& filepath,
                                const tp::domain::Scenario& scenario);
    
    /**
     * @brief Genera nombre de archivo con timestamp
     * 
     * Crea un nombre único basado en prefijo y fecha/hora actual.
     * 
     * @param prefix Prefijo del nombre (ej: "simulation", "trajectory")
     * @return Nombre de archivo generado (ej: "simulation_20240325_143052.json")
     */
    static std::string generateFilename(const std::string& prefix = "simulation");
    
    /**
     * @brief Exporta comparación de múltiples métodos numéricos
     * 
     * Genera un JSON con resultados de diferentes métodos de integración
     * para análisis comparativo de precisión y rendimiento.
     * 
     * @param filepath Ruta del archivo de salida
     * @param results Vector de pares (método, resultado)
     * @return true si la exportación fue exitosa
     */
    static bool exportMethodComparison(
        const std::string& filepath,
        const std::vector<std::pair<std::string, tp::simulation::SimulationResult>>& results);

private:
    /**
     * @brief Escapa caracteres especiales para JSON
     */
    static std::string escapeJson(const std::string& str);
    
    /**
     * @brief Formatea un número con precisión controlada
     */
    static std::string formatDouble(double value, int precision = 6);
};

} // namespace tp::exporting
