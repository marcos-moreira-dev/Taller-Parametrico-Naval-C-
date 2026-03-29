#pragma once

/**
 * @file Integrators.hpp
 * @brief Métodos numéricos de integración para simulaciones
 * @author Proyecto Taller Paramétrico
 * @version 0.2.3
 * 
 * Define la interfaz y implementaciones de integradores numéricos
 * para resolver las ecuaciones diferenciales del movimiento del bateau.
 * 
 * Métodos disponibles:
 * - Euler: primer orden (rápido, poco preciso)
 * - Improved Euler (Heun): segundo orden
 * - Runge-Kutta 4: cuarto orden (preciso, estándar)
 * 
 * @see PhysicsModel para el cálculo de fuerzas
 * @see SimulationRunner para el uso de integradores
 */

#include "models/PhysicsModel.hpp"

namespace tp::simulation {

using namespace tp::shared;

/**
 * @brief Interfaz abstracta para integradores numéricos
 * 
 * Define el contrato que deben cumplir todos los métodos
 * de integración numérica para la simulación.
 */
class Integrator {
public:
    virtual ~Integrator() = default;
    
    /**
     * @brief Ejecuta un paso de integración
     * @param current Estado actual del sistema (posición, velocidad, tiempo)
     * @param model Modelo físico para calcular aceleraciones
     * @param dt Paso temporal [s]
     * @return Nuevo estado después del paso
     * 
     * Cada integrador implementa su propio algoritmo:
     * - Euler: y_{n+1} = y_n + f(t_n, y_n) * dt
     * - Heun: promedio de pendientes
     * - RK4: promedio ponderado de 4 evaluaciones
     */
    virtual PhysicsState step(const PhysicsState& current, const PhysicsModel& model,
                                  double dt) = 0;
    
    /**
     * @brief Nombre del método para display
     * @return Nombre descriptivo del integrador
     */
    virtual const char* getName() const = 0;
};

/**
 * @brief Integrador de Euler (método de primer orden)
 * 
 * El método más simple pero menos preciso. Apropiado para:
 * - Demostraciones rápidas
 * - Pasos temporales muy pequeños
 * - Comparaciones pedagógicas con otros métodos
 */
class EulerIntegrator : public Integrator {
public:
    PhysicsState step(const PhysicsState& current, const PhysicsModel& model,
                         double dt) override;
    const char* getName() const override { return "Euler"; }
};

/**
 * @brief Integrador de Euler Mejorado (método de Heun)
 *
 * Segundo orden de precisión. Utiliza un paso predictor-corrector
 * que mejora significativamente la precisión respecto a Euler simple.
 */
class ImprovedEulerIntegrator : public Integrator {
public:
    PhysicsState step(const PhysicsState& current, const PhysicsModel& model,
                         double dt) override;
    const char* getName() const override { return "Improved Euler (Heun)"; }
};

/**
 * @brief Integrador Runge-Kutta de cuarto orden (RK4)
 *
 * Método estándar de alta precisión para ecuaciones diferenciales ordinarias.
 * Evalúa cuatro veces la derivada por paso, proporcionando excelente
 * relación entre costo computacional y precisión (error O(h⁵)).
 */
class RK4Integrator : public Integrator {
public:
    PhysicsState step(const PhysicsState& current, const PhysicsModel& model,
                         double dt) override;
    const char* getName() const override { return "Runge-Kutta 4"; }
};

}
