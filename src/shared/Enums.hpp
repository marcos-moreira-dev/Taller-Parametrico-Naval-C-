#pragma once

/**
 * @file Enums.hpp
 * @brief Enumeraciones globales del proyecto
 * @author Proyecto Taller Paramétrico
 * @version 0.2.3
 * 
 * Define enumeraciones usadas en múltiples módulos:
 * - CellType: tipos de celdas del escenario
 * - SimulationState: estados de la simulación
 * - IntegrationMethod: métodos numéricos disponibles
 */

namespace tp::shared {

/**
 * @brief Tipos de celdas en el escenario
 * 
 * Define el contenido de cada celda del grid del escenario.
 */
enum class CellType {
    Water,      ///< Celda de agua (azul, navegable)
    Land,       ///< Celda de tierra (verde, obstáculo)
    Obstacle    ///< Obstáculo duro (gris, colisión)
};

/**
 * @brief Estados de la simulación
 * 
 * Controla el ciclo de vida de la simulación.
 */
enum class SimulationState {
    Stopped,    ///< Simulación no iniciada o reiniciada
    Running,    ///< Ejecutando pasos de simulación
    Paused,     ///< Pausada pero puede continuar
    Finished    ///< Completada (tiempo o colisión)
};

/**
 * @brief Métodos de integración numérica
 * 
 * Algoritmos para resolver ecuaciones diferenciales.
 */
enum class IntegrationMethod {
    Euler,          ///< Primer orden - rápido, poco preciso
    ImprovedEuler,  ///< Segundo orden - balance
    RK4             ///< Cuarto orden - preciso, estándar
};

}
