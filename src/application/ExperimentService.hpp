#pragma once

/**
 * @file ExperimentService.hpp
 * @brief Servicio central de gestión de experimentos y simulaciones
 * @author Proyecto Taller Paramétrico
 * @version 0.2.3
 * 
 * Proporciona una interfaz unificada para:
 * - Gestionar la configuración del experimento (escenario, campo, bateau)
 * - Ejecutar simulaciones con diferentes métodos numéricos
 * - Mantener escenarios predefinidos (presets)
 * - Notificar cambios a componentes dependientes (Observer pattern)
 * 
 * @note Usa el patrón Observer para mantener sincronizada la UI
 * @see MainWindow para integración con la interfaz
 * @see SimulationRunner para la ejecución de simulaciones
 */

#include "Scenario.hpp"
#include "VectorField.hpp"
#include "Boat.hpp"
#include "Motor.hpp"
#include "runner/SimulationRunner.hpp"
#include <memory>
#include <vector>
#include <functional>

namespace tp::application {

using namespace tp::domain;
using namespace tp::shared;

enum class FieldPresetType {
    Uniform = 0,
    Linear = 1,
    Radial = 2,
    Rotational = 3,
    Custom = 4
};

struct FieldConfigView {
    FieldPresetType type = FieldPresetType::Uniform;
    double intensity = 0.0;
    double centerX = 25.0;
    double centerY = 25.0;
    std::string customFx = "0";
    std::string customFy = "0";
};

/**
 * @brief Configuración completa de un experimento
 * 
 * Agrupa todos los parámetros necesarios para ejecutar una simulación:
 * - Escenario: definición del terreno (células de agua/tierra/obstáculos)
 * - Campo vectorial: comportamiento del fluido
 * - Bateau: objeto simulado
 * - Motor: propulsión del bateau
 * - Parámetros numéricos: paso temporal, método de integración, duración
 */
struct ExperimentConfig {
    Scenario scenario;               ///< Definición del escenario (50x50 por defecto)
    VectorField field;               ///< Campo vectorial de corrientes
    FieldConfigView fieldView;       ///< Parámetros explícitos del campo para la UI
    Boat boat;                       ///< Estado inicial del bateau
    Motor motor;                     ///< Configuración del motor
    double timeStep = 0.01;         ///< Paso temporal [s]
    double maxTime = 10.0;           ///< Tiempo máximo de simulación [s]
    IntegrationMethod method = IntegrationMethod::Euler;  ///< Método numérico
    
    ExperimentConfig() : scenario(50, 50) {}
};

/**
 * @brief Representa un escenario predefinido cargable
 * 
 * Cada preset contiene:
 * - Nombre descriptivo para mostrar en UI
 * - Función de configuración que modifica ExperimentConfig
 */
struct ScenarioPreset {
    std::string name;                                             ///< Nombre visible del preset
    std::function<void(ExperimentConfig&)> configure;            ///< Función de configuración
};

/**
 * @brief Servicio central de experimentos
 * 
 * Maneja el ciclo de vida completo de un experimento:
 * 1. Configuración inicial (presets)
 * 2. Ejecución de simulación
 * 3. Notificación de cambios a observadores
 * 
 * Implementa el patrón Observer para mantener sincronizada la UI.
 * 
 * Ejemplo de uso:
 * @code
 * ExperimentService service;
 * service.applyScenarioPreset(0);  // Cargar escenario "Río uniforme"
 * auto result = service.runSimulation();  // Ejecutar simulación
 * @endcode
 */
class ExperimentService {
public:
    /**
     * @brief Constructor - inicializa con valores por defecto
     * 
     * Crea un experimento básico con:
     * - Escenario de 50x50 células de agua
     * - Campo vectorial nulo
     * - Bateau con propiedades por defecto
     * - Motor apagado
     * - Método Euler, dt=0.01s, maxTime=10s
     */
    ExperimentService();
    
    // ============================================================
    // GESTIÓN DEL EXPERIMENTO
    // ============================================================
    
    /**
     * @brief Reinicia el experimento a estado inicial
     * 
     * Restablece:
     * - Escenario vacío (50x50 agua)
     * - Campo vectorial nulo
     * - Bateau en posición (0,0)
     * - Motor apagado
     */
    void createNewExperiment();
    
    /**
     * @brief Carga un experimento desde archivo
     * @param filepath Ruta al archivo de experimento
     * 
     */
    bool loadExperiment(const std::string& filepath);
    
    /**
     * @brief Guarda el experimento actual a archivo
     * @param filepath Ruta destino
     * 
     * @note Por implementar: requiere ScenarioSaver
     */
    void saveExperiment(const std::string& filepath);
    
    // ============================================================
    // SETTERS DE COMPONENTES
    // ============================================================
    
    /**
     * @brief Actualiza el escenario
     * @param scenario Nuevo escenario (copia)
     * 
     * Notifica a todos los observadores del cambio.
     */
    void setScenario(const Scenario& scenario);
    
    /**
     * @brief Actualiza el campo vectorial
     * @param field Nuevo campo (copia)
     * 
     * Notifica a todos los observadores del cambio.
     */
    void setField(const VectorField& field);
    void setFieldView(const FieldConfigView& fieldView);
    bool rebuildFieldFromView(std::string* errorMessage = nullptr);
    
    /**
     * @brief Actualiza el bateau
     * @param boat Nuevo estado del bateau (copia)
     * 
     * Notifica a todos los observadores del cambio.
     */
    void setBoat(const Boat& boat);
    
    /**
     * @brief Actualiza la configuración del motor
     * @param motor Nuevo motor (copia)
     * 
     * Notifica a todos los observadores del cambio.
     */
    void setMotor(const Motor& motor);
    
    // ============================================================
    // EJECUCIÓN DE SIMULACIÓN
    // ============================================================
    
    /**
     * @brief Ejecuta la simulación completa
     * @return Resultado con estados, trayectoria y métricas
     * 
     * Usa el método de integración y paso temporal configurados.
     * Ejecuta desde t=0 hasta t=maxTime o hasta colisión.
     */
    simulation::SimulationResult runSimulation();
    
    // ============================================================
    // ACCESSORS
    // ============================================================
    
    /**
     * @brief Obtiene referencia mutable a la configuración actual
     * @return Referencia a ExperimentConfig
     * 
     * @warning Preferir setters individuales para notificar cambios
     */
    ExperimentConfig& getConfig() { return config_; }
    
    /**
     * @brief Obtiene referencia constante a la configuración
     * @return Referencia constante a ExperimentConfig
     */
    const ExperimentConfig& getConfig() const { return config_; }
    
    // ============================================================
    // ESCENARIOS PREDEFINIDOS (PRESETS)
    // ============================================================
    
    /**
     * @brief Cantidad de escenarios predefinidos disponibles
     * @return Número de presets
     */
    size_t getScenarioPresetCount() const { return presets_.size(); }
    
    /**
     * @brief Obtiene el nombre de un preset por índice
     * @param index Índice del preset [0, getScenarioPresetCount())
     * @return Nombre del preset
     * 
     * @throws std::out_of_range si índice inválido
     */
    std::string getScenarioPresetName(size_t index) const;
    
    /**
     * @brief Aplica un preset al experimento actual
     * @param index Índice del preset a aplicar
     * 
     * Ejecuta la función de configuración del preset,
     * actualizando todos los componentes del experimento.
     */
    void applyScenarioPreset(size_t index);
    
    // ============================================================
    // PATRÓN OBSERVER
    // ============================================================
    
    /**
     * @brief Callback para notificación de cambios
     * 
     * Se invoca cada vez que se modifica la configuración
     * a través de los setters setScenario/setField/setBoat/setMotor.
     */
    using ConfigChangeCallback = std::function<void()>;
    
    /**
     * @brief Registra un observador para cambios de configuración
     * @param callback Función a ejecutar en cada cambio
     * 
     * Ejemplo de uso:
     * @code
     * service.addConfigChangeListener([this]() {
     *     rightPanel_->updateProperties();
     * });
     * @endcode
     */
    void addConfigChangeListener(ConfigChangeCallback callback) {
        configChangeCallbacks_.push_back(callback);
    }
    
    /**
     * @brief Notifica a todos los observadores
     * 
     * Invoca todos los callbacks registrados.
     * Se llama automáticamente después de setScenario/setField/setBoat/setMotor.
     */
    void notifyConfigChange() {
        for (auto& callback : configChangeCallbacks_) {
            callback();
        }
    }

private:
    /**
     * @brief Inicializa la lista de escenarios predefinidos
     * 
     * Carga los 10 presets disponibles:
     * 1. Río uniforme
     * 2. Remolino central
     * 3. Canal con obstáculo
     * 4. Zona turbulenta
     * 5. Laberinto de canales
     * 6. Zona de rápidos
     * 7. Efecto Venturi
     * 8. Puerto con rompeolas
     * 9. Corrientes en capas
     * 10. Corriente oscilatoria
     */
    void initPresets();
    
    ExperimentConfig config_;                                     ///< Configuración actual
    std::unique_ptr<simulation::SimulationRunner> runner_;       ///< Ejecutor de simulación
    std::vector<ScenarioPreset> presets_;                         ///< Escenarios predefinidos
    std::vector<ConfigChangeCallback> configChangeCallbacks_;    ///< Observadores registrados
};

} // namespace tp::application
