#pragma once

#include "ExperimentService.hpp"
#include "runner/SimulationRunner.hpp"
#include <functional>
#include <vector>

namespace tp::application {

/**
 * @brief Estado actual de la simulación
 */
enum class SimulationState {
    Stopped,
    Running,
    Paused,
    Finished
};

/**
 * @brief Punto de trayectoria para análisis
 */
struct TrajectoryPoint {
    double x;
    double y;
    double time;
    double velocity;
    double energy;
};

/**
 * @brief Estadísticas de la simulación
 */
struct SimulationStats {
    double totalTime;
    double totalDistance;
    double maxSpeed;
    double averageSpeed;
    double finalEnergy;
};

/**
 * @brief Callback para cambios de estado
 */
using StateCallback = std::function<void(SimulationState)>;

/**
 * @brief Callback para actualizaciones de frame
 */
using UpdateCallback = std::function<void(const simulation::PhysicsState&)>;

/**
 * @brief Coordina la ejecución de la simulación física
 * 
 * Gestiona el ciclo de vida de la simulación: inicio, pausa, reinicio,
 * y proporciona callbacks para actualizar la UI en tiempo real.
 */
class SimulationCoordinator {
public:
    SimulationCoordinator();
    
    /**
     * @brief Inicializa el coordinador con una configuración
     */
    void initialize(const ExperimentConfig& config);
    
    /**
     * @brief Inicia la simulación
     */
    void start();
    
    /**
     * @brief Pausa/reanuda la simulación
     */
    void pause();
    
    /**
     * @brief Detiene la simulación
     */
    void stop();
    
    /**
     * @brief Reinicia la simulación a estado inicial
     */
    void reset();
    
    /**
     * @brief Ejecuta un paso de simulación
     * @return true si la simulación puede continuar
     */
    bool step();
    
    /**
     * @brief Ejecuta la simulación completa hasta el final
     */
    void runComplete();
    
    /**
     * @brief Cambia el método de integración numérica
     */
    void setIntegrationMethod(IntegrationMethod method);
    
    /**
     * @brief Obtiene el estado actual
     */
    SimulationState getCurrentState() const { return currentState_; }
    
    /**
     * @brief Obtiene el tiempo actual de simulación
     */
    double getCurrentTime() const { return currentTime_; }
    
    /**
     * @brief Obtiene la trayectoria acumulada
     */
    const std::vector<TrajectoryPoint>& getTrajectory() const { return trajectory_; }
    
    /**
     * @brief Obtiene estadísticas de la simulación
     */
    SimulationStats getStatistics() const;
    
    /**
     * @brief Registra callback para cambios de estado
     */
    void registerStateCallback(StateCallback callback);
    
    /**
     * @brief Registra callback para actualizaciones
     */
    void registerUpdateCallback(UpdateCallback callback);
    
private:
    void updateConfiguration();
    
    ExperimentConfig config_;
    std::unique_ptr<simulation::SimulationRunner> runner_;
    
    SimulationState currentState_;
    bool isPaused_;
    double currentTime_;
    double timeStep_;
    double maxTime_;
    IntegrationMethod currentMethod_;
    
    std::vector<TrajectoryPoint> trajectory_;
    std::vector<StateCallback> stateCallbacks_;
    std::vector<UpdateCallback> updateCallbacks_;
};

} // namespace tp::application
