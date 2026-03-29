#include "Boat.hpp"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace tp::domain {

using namespace tp::shared;

Boat::Boat()
    : position_(0.0, 0.0)
    , velocity_(0.0, 0.0)
    , orientation_(0.0)
    , angularVelocity_(0.0)
    , mass_(10.0)
    , size_(5.0)
    , dragCoefficient_(1.0)
    , inertia_(100.0)
    , grounded_(false)
    , acceleration_(0.0, 0.0)
    , angularAcceleration_(0.0)
{
    updateHull();
}

void Boat::reset() {
    velocity_ = Vec2d(0.0, 0.0);
    angularVelocity_ = 0.0;
    acceleration_ = Vec2d(0.0, 0.0);
    angularAcceleration_ = 0.0;
    grounded_ = false;
}

void Boat::updateHull() {
    // Actualizar propiedades del casco basadas en el tamaño
    hull_.length = size_ * 1.5;
    hull_.beam = size_ * 0.6;
    hull_.draft = size_ * 0.15;
    // Área mojada aproximada
    hull_.wettedArea = (2.0 * hull_.length + 2.0 * hull_.beam) * hull_.draft;
    updateInertia();
}

void Boat::updateInertia() {
    // Momento de inercia para una lámina rectangular
    inertia_ = (1.0/12.0) * mass_ * (hull_.length * hull_.length + hull_.beam * hull_.beam);
}

double Boat::getSpeedKnots() const {
    // 1 m/s ≈ 1.94384 knots
    return velocity_.magnitude() * 1.94384;
}

double Boat::getHeading() const {
    // Dirección real de movimiento
    if (velocity_.magnitude() < 0.001) {
        return orientation_;
    }
    return std::atan2(velocity_.y, velocity_.x);
}

void Boat::applyForce(const Vec2d& force, double dt) {
    // Segunda ley de Newton: F = ma
    acceleration_ = force * (1.0 / mass_);
    velocity_ = velocity_ + acceleration_ * dt;
    position_ = position_ + velocity_ * dt;
}

void Boat::applyTorque(double torque, double dt) {
    // τ = Iα
    angularAcceleration_ = torque / inertia_;
    angularVelocity_ += angularAcceleration_ * dt;
    orientation_ += angularVelocity_ * dt;
    
    // Normalizar orientación a [-π, π]
    while (orientation_ > M_PI) orientation_ -= 2.0 * M_PI;
    while (orientation_ < -M_PI) orientation_ += 2.0 * M_PI;
}

void Boat::applyHydrodynamicForces(const Vec2d& waterVelocity, double waterDensity, double dt) {
    if (grounded_) {
        // Si está encallado, aplicar fricción muy alta
        velocity_ = velocity_ * 0.1;
        angularVelocity_ *= 0.1;
        return;
    }
    
    // Velocidad relativa al agua
    Vec2d relativeVel = velocity_ - waterVelocity;
    double speed = relativeVel.magnitude();
    
    if (speed < 0.001) return;
    
    // Dirección opuesta a la velocidad relativa
    Vec2d dragDirection = relativeVel * (-1.0 / speed);
    
    // Fuerza de arrastre viscoso (lineal a bajas velocidades)
    double viscousDragCoeff = 0.5 * waterDensity * hull_.wettedArea * dragCoefficient_;
    Vec2d viscousDrag = dragDirection * (viscousDragCoeff * speed * 0.1);
    
    // Fuerza de arrastre cuadrático (dominante a altas velocidades)
    double quadraticDragCoeff = 0.5 * waterDensity * dragCoefficient_ * hull_.wettedArea;
    Vec2d quadraticDrag = dragDirection * (quadraticDragCoeff * speed * speed);
    
    // Combinar arrastres
    double reynoldsNumber = speed * hull_.length / 1.0e-6;
    double laminarFactor = std::min(1.0, 100000.0 / std::max(reynoldsNumber, 1000.0));
    
    Vec2d totalDrag = viscousDrag * laminarFactor + quadraticDrag * (1.0 - laminarFactor);
    
    // Fuerza de sustentación hidrodinámica (dinámica)
    double froudeNumber = speed / std::sqrt(9.81 * hull_.length);
    double liftFactor = std::max(0.3, 1.0 - froudeNumber * froudeNumber * 0.5);
    hull_.wettedArea = (2.0 * hull_.length + 2.0 * hull_.beam) * hull_.draft * liftFactor;
    
    // Fuerza de Coriolis
    double coriolisForce = 2.0 * mass_ * angularVelocity_ * speed;
    Vec2d coriolisDir(-relativeVel.y, relativeVel.x);
    if (coriolisDir.magnitude() > 0.001) {
        coriolisDir = coriolisDir * (1.0 / coriolisDir.magnitude());
    }
    Vec2d coriolis = coriolisDir * coriolisForce * 0.1;
    
    // Aplicar fuerzas
    Vec2d totalForce = totalDrag + coriolis;
    applyForce(totalForce, dt);
    
    // Torque de alineación
    double angleOfAttack = std::atan2(relativeVel.y, relativeVel.x) - orientation_;
    while (angleOfAttack > M_PI) angleOfAttack -= 2.0 * M_PI;
    while (angleOfAttack < -M_PI) angleOfAttack += 2.0 * M_PI;
    
    double restoringTorque = -0.5 * waterDensity * speed * speed * hull_.beam * hull_.length * 
                            std::sin(2.0 * angleOfAttack) * 0.1;
    
    double dampingTorque = -0.1 * angularVelocity_ * mass_;
    
    applyTorque(restoringTorque + dampingTorque, dt);
}

}
