/**
 * @file PhysicsModel.hpp
 * @brief Modelo físico para simulación de barco
 * @author Proyecto Taller Paramétrico
 * @version 0.2.3
 * 
 * Implementa el cálculo de fuerzas, aceleraciones y detección de colisiones
 * para la simulación del barco en un campo vectorial.
 * 
 * @see VectorField para el campo de fuerzas
 * @see Boat para el modelo del barco
 * @see Motor para la propulsión
 * @see Integrators.hpp para la integración numérica
 */

#pragma once

#include "Vec2.hpp"
#include "Boat.hpp"
#include "VectorField.hpp"
#include "Motor.hpp"
#include "Scenario.hpp"

namespace tp::simulation {

using namespace tp::shared;
using namespace tp::domain;

/**
 * @brief Estado de la simulación física en un momento dado
 * 
 * Contiene toda la información dinámica del sistema: posición, velocidad,
 * energías y estado de colisión. Se distingue del enum SimulationState
 * que representa el estado de ejecución (Stopped, Running, etc.).
 * 
 * @see SimulationCoordinator para el estado de ejecución
 */
struct PhysicsState {
    double time;              ///< Tiempo actual de la simulación [s]
    Vec2d position;           ///< Posición (x,y) [m]
    Vec2d velocity;           ///< Velocidad lineal [m/s]
    double orientation;       ///< Ángulo de orientación [rad]
    double kineticEnergy;     ///< Energía cinética [J]
    double totalEnergy;       ///< Energía total del sistema [J]
    double accumulatedWork;   ///< Trabajo acumulado [J]
    bool collision;           ///< Indica si hay colisión
    
    PhysicsState()
        : time(0.0)
        , position(0.0, 0.0)
        , velocity(0.0, 0.0)
        , orientation(0.0)
        , kineticEnergy(0.0)
        , totalEnergy(0.0)
        , accumulatedWork(0.0)
        , collision(false)
    {}
};

/**
 * @brief Modelo físico para simulación de dinámica marinera
 * 
 * Calcula aceleraciones, potencias y detecta colisiones del barco
 * en un campo vectorial given. Utiliza las propiedades del barco,
 * el campo de fuerzas y el motor para determinar el comportamiento físico.
 */
class PhysicsModel {
public:
    /**
     * @brief Constructor del modelo físico
     * @param boat Referencia al modelo del barco
     * @param field Referencia al campo vectorial de fuerzas
     * @param motor Referencia al modelo de propulsión
     */
    PhysicsModel(const Boat& boat, const VectorField& field, const Motor& motor);
    
    /**
     * @brief Asigna el escenario para detección de colisiones
     * @param scenario Puntero al escenario (puede ser nullptr)
     */
    void setScenario(const Scenario* scenario) { scenario_ = scenario; }
    
    /**
     * @brief Calcula la aceleración en una posición y velocidad dadas
     * @param position Posición actual [m]
     * @param velocity Velocidad actual [m/s]
     * @param time Tiempo actual [s]
     * @return Vector de aceleración [m/s²]
     * 
     * Calcula la aceleración total considerando:
     * - Fuerza del campo vectorial
     * - Propulsión del motor
     * - Fricción y resistencia fluida
     */
    Vec2d computeAcceleration(const Vec2d& position, const Vec2d& velocity, double time) const;
    
    /**
     * @brief Calcula la potencia instantánea del motor
     * @param velocity Velocidad actual [m/s]
     * @param time Tiempo actual [s]
     * @return Potencia entregada [W]
     */
    double computePower(const Vec2d& velocity, double time) const;
    
    /**
     * @brief Verifica colisión con obstáculos del escenario
     * @param position Posición a verificar [m]
     * @return true si hay colisión
     */
    bool checkCollision(const Vec2d& position) const;
    
private:
    const Boat& boat_;
    const VectorField& field_;
    const Motor& motor_;
    const Scenario* scenario_;
};

}
