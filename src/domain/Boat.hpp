#pragma once

/**
 * @file Boat.hpp
 * @brief Modelo físico avanzado de embarcación para simulación hidrodinámica
 * @author Proyecto Taller Paramétrico
 * @version 0.2.0
 * 
 * Implementa física de fluidos computacional simplificada:
 * - Dinámica de cuerpo rígido 2D
 * - Fuerzas hidrodinámicas (arrastre, sustentación)
 * - Números adimensionales (Reynolds, Froude)
 * - Comportamiento de planeo y estabilidad
 * 
 * @see PhysicsModel para integración con sistema de fuerzas
 * @see SimulationRunner para uso en simulación
 */

// Definir ssize_t si no está definido
#if !defined(ssize_t) && !defined(_SSIZE_T_DEFINED)
#define ssize_t long
#define _SSIZE_T_DEFINED
#endif

#include "Vec2.hpp"

namespace tp::domain {

using namespace tp::shared;

/**
 * @brief Propiedades geométricas del casco para cálculos hidrodinámicos
 * 
 * Estas propiedades afectan:
 * - Resistencia al avance (arrastre)
 * - Estabilidad direccional
 * - Capacidad de planeo
 * - Momento de inercia
 */
struct HullProperties {
    double length;              ///< Longitud del casco [m]
    double beam;               ///< Manga (ancho máximo) [m]
    double draft;              ///< Calado (profundidad en agua) [m]
    double blockCoefficient;   ///< Coeficiente de bloque (0.4-0.8)
    double wettedArea;         ///< Área superficial mojada [m²]
    
    HullProperties() 
        : length(5.0)
        , beam(2.0)
        , draft(0.5)
        , blockCoefficient(0.6)
        , wettedArea(8.0)
    {}
};

/**
 * @brief Representa una embarcación con física hidrodinámica realista
 * 
 * Modela el comportamiento de un bote en agua considerando:
 * - Fuerzas de propulsión (motor)
 * - Arrastre viscoso y de forma
 * - Sustentación hidrodinámica (dinámica)
 * - Efectos de giro y estabilidad
 * - Colisión con obstáculos (encallamiento)
 */
class Boat {
public:
    /**
     * @brief Constructor - inicializa bote con valores por defecto
     */
    Boat();
    
    // ============================================================
    // GETTERS DE ESTADO
    // ============================================================
    
    Vec2d getPosition() const { return position_; }
    Vec2d getVelocity() const { return velocity_; }
    double getOrientation() const { return orientation_; }
    double getAngularVelocity() const { return angularVelocity_; }
    double getMass() const { return mass_; }
    double getSize() const { return size_; }
    double getDragCoefficient() const { return dragCoefficient_; }
    
    /**
     * @brief Obtiene propiedades del casco para cálculos avanzados
     * @return Referencia constante a propiedades del casco
     */
    const HullProperties& getHullProperties() const { return hull_; }
    
    /**
     * @brief Momento de inercia rotacional calculado a partir de la geometría
     * @return I [kg·m²]
     */
    double getInertia() const { return inertia_; }
    
    /**
     * @brief Verifica si el bote está encallado (tocando tierra)
     * @return true si está en contacto con obstáculos
     */
    bool isGrounded() const { return grounded_; }
    
    /**
     * @brief Velocidad en nudos (unidades náuticas)
     * @return Velocidad [nudos] (1 m/s ≈ 1.94384 nudos)
     */
    double getSpeedKnots() const;
    
    /**
     * @brief Dirección real de movimiento (heading)
     * @return Ángulo de trayectoria [rad] - puede diferir de orientación
     */
    double getHeading() const;
    
    // ============================================================
    // SETTERS
    // ============================================================
    
    void setPosition(const Vec2d& pos) { position_ = pos; }
    void setVelocity(const Vec2d& vel) { velocity_ = vel; }
    void setOrientation(double angle) { orientation_ = angle; }
    void setAngularVelocity(double omega) { angularVelocity_ = omega; }
    
    /**
     * @brief Cambia la masa y actualiza automáticamente el momento de inercia
     * @param mass Nueva masa [kg]
     */
    void setMass(double mass) { mass_ = mass; updateInertia(); }
    
    /**
     * @brief Cambia el tamaño y actualiza propiedades del casco
     * @param size Factor de escala general
     */
    void setSize(double size) { size_ = size; updateHull(); }
    
    void setDragCoefficient(double coeff) { dragCoefficient_ = coeff; }
    void setGrounded(bool grounded) { grounded_ = grounded; }
    
    // ============================================================
    // FÍSICA
    // ============================================================
    
    /**
     * @brief Aplica fuerza según 2da ley de Newton (F=ma)
     * @param force Vector de fuerza [N]
     * @param dt Paso temporal [s]
     */
    void applyForce(const Vec2d& force, double dt);
    
    /**
     * @brief Aplica torque para rotación (τ=Iα)
     * @param torque Momento de torsión [N·m]
     * @param dt Paso temporal [s]
     */
    void applyTorque(double torque, double dt);
    
    /**
     * @brief Aplica todas las fuerzas hidrodinámicas
     * 
     * Incluye:
     * - Arrastre viscoso (lineal)
     * - Arrastre cuadrático (turbulento)
     * - Sustentación hidrodinámica (planeo)
     * - Efecto Coriolis
     * - Torque de alineación
     * 
     * @param waterVelocity Velocidad del agua en posición actual [m/s]
     * @param waterDensity Densidad del agua [kg/m³]
     * @param dt Paso temporal [s]
     */
    void applyHydrodynamicForces(const Vec2d& waterVelocity, double waterDensity, double dt);
    
    /**
     * @brief Reinicia estado dinámico (velocidades, aceleraciones)
     * Mantiene posición y propiedades físicas
     */
    void reset();
    
private:
    /**
     * @brief Recalcula momento de inercia a partir de masa y geometría
     */
    void updateInertia();
    
    /**
     * @brief Actualiza propiedades del casco basadas en tamaño
     */
    void updateHull();
    
    // ============================================================
    // MIEMBROS DE ESTADO
    // ============================================================
    
    // Cinemática
    Vec2d position_;           ///< Posición (x,y) [m]
    Vec2d velocity_;           ///< Velocidad lineal [m/s]
    double orientation_;       ///< Ángulo de orientación [rad]
    double angularVelocity_;   ///< Velocidad angular [rad/s]
    
    // Propiedades físicas
    double mass_;              ///< Masa total [kg]
    double size_;              ///< Factor de escala
    double dragCoefficient_;   ///< Coeficiente de arrastre
    double inertia_;           ///< Momento de inercia I [kg·m²]
    
    // Propiedades avanzadas
    HullProperties hull_;      ///< Geometría del casco
    bool grounded_;            ///< Estado de encallamiento
    
    // Variables internas de cálculo
    Vec2d acceleration_;       ///< Aceleración lineal actual [m/s²]
    double angularAcceleration_; ///< Aceleración angular [rad/s²]
};

} // namespace tp::domain
