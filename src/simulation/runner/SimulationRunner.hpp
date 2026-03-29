#pragma once

/**
 * @file SimulationRunner.hpp
 * @brief Ejecutor de simulaciones físicas
 * @author TallerParamétrico
 * @version 1.0
 * @see PhysicsModel Integrators
 */

#include <vector>
#include <memory>
#include "integrators/Integrators.hpp"
#include "Boat.hpp"
#include "VectorField.hpp"
#include "Motor.hpp"
#include "Scenario.hpp"
#include "Enums.hpp"

namespace tp::simulation {

using namespace tp::domain;
using namespace tp::shared;

/**
 * @brief Resultado completo de una simulación ejecutada
 *
 * Contiene la trayectoria completa, métricas de rendimiento y
 * metadatos sobre cómo se ejecutó la simulación.
 */
struct SimulationResult {
    std::vector<PhysicsState> states;  ///< Historial de estados de la simulación
    double finalTime;                  ///< Tiempo final alcanzado [s]
    double totalWork;                  ///< Trabajo total realizado [J]
    double maxSpeed;                   ///< Velocidad máxima alcanzada [m/s]
    double totalDistance;              ///< Distancia total recorrida [m]
    bool endedByCollision;             ///< Indica si terminó por colisión
    IntegrationMethod methodUsed;      ///< Método numérico utilizado
};

/**
 * @brief Ejecutor de simulaciones físicas
 * @class SimulationRunner
 * @details Gestiona la ejecución de la simulación utilizando un integrador
 * numérico, acumulando estados y calculando métricas finales.
 */
class SimulationRunner {
public:
    /**
     * @brief Constructor del ejecutor
     * @param boat Modelo de la embarcacion
     * @param field Campo de fuerzas
     * @param motor Modelo del motor
     * @param scenario Escenario de la simulacion
     */
    SimulationRunner(const Boat& boat, const VectorField& field, const Motor& motor,
                     const Scenario& scenario);
    
    /**
     * @brief Establece el integrador numérico
     * @param integrator Integrador a utilizar
     */
    void setIntegrator(std::unique_ptr<Integrator> integrator);
    
    /**
     * @brief Establece el paso de tiempo
     * @param dt Paso de tiempo [s]
     */
    void setTimeStep(double dt) { timeStep_ = dt; }
    
    /**
     * @brief Establece el tiempo máximo de simulación
     * @param maxTime Tiempo máximo [s]
     */
    void setMaxTime(double maxTime) { maxTime_ = maxTime; }
    
    /**
     * @brief Ejecuta la simulación completa
     * @return Resultado con todos los estados y métricas
     */
    SimulationResult run();
    
    /**
     * @brief Reinicia la simulación a estado inicial
     */
    void reset();
    
    /**
     * @brief Pausa la simulación
     */
    void pause() { paused_ = true; }
    
    /**
     * @brief Reanuda la simulación pausada
     */
    void resume() { paused_ = false; }
    
    /**
     * @brief Detiene la simulación
     */
    void stop() { stopped_ = true; }
    
    /**
     * @brief Consulta si la simulación está en ejecución
     */
    bool isRunning() const { return running_; }
    
    /**
     * @brief Consulta si la simulación está pausada
     */
    bool isPaused() const { return paused_; }
    
private:
    const Boat& boat_;
    const VectorField& field_;
    const Motor& motor_;
    const Scenario& scenario_;
    
    std::unique_ptr<Integrator> integrator_;
    double timeStep_;
    double maxTime_;
    
    bool running_;
    bool paused_;
    bool stopped_;
};

}
