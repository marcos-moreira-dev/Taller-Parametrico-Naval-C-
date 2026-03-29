#pragma once

/**
 * @file Motor.hpp
 * @brief Modelo de propulsión del bateau
 * @author Proyecto Taller Paramétrico
 * @version 0.2.3
 * 
 * Define el comportamiento del motor del bateau:
 * - Empuje constante (configuración simple)
 * - Empuje variable (función del tiempo)
 * - Empuje sinusoidal (para simulate ondas)
 * 
 * @see Boat para integración con física
 */

#include <functional>
#include "Vec2.hpp"

namespace tp::domain {

using namespace tp::shared;

class Motor {
public:
    using ThrustFunction = std::function<double(double time)>;
    
    Motor();
    
    double getThrustMagnitude(double time) const;
    Vec2d getThrustVector(double time) const;
    
    double getDirection() const { return direction_; }
    void setDirection(double angle) { direction_ = angle; }
    
    void setConstantThrust(double magnitude);
    void setThrustFunction(ThrustFunction func);
    
    static Motor constant(double magnitude, double direction);
    static Motor sinusoidal(double baseThrust, double amplitude, double frequency, double direction);
    
private:
    double constantThrust_;
    ThrustFunction thrustFunction_;
    double direction_;
    bool useFunction_;
};

}
