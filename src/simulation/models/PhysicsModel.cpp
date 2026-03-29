#include "PhysicsModel.hpp"
#include <cmath>

namespace tp::simulation {

PhysicsModel::PhysicsModel(const Boat& boat, const VectorField& field, const Motor& motor)
    : boat_(boat)
    , field_(field)
    , motor_(motor)
    , scenario_(nullptr) {}

Vec2d PhysicsModel::computeAcceleration(const Vec2d& position, const Vec2d& velocity, double time) const {
    Vec2d fieldVelocity = field_.getValue(position);
    Vec2d relativeVelocity = velocity - fieldVelocity;
    
    double dragCoeff = boat_.getDragCoefficient();
    Vec2d dragForce = relativeVelocity * (-dragCoeff);
    
    Vec2d thrust = motor_.getThrustVector(time);
    
    Vec2d totalForce = thrust + dragForce;
    
    return totalForce / boat_.getMass();
}

double PhysicsModel::computePower(const Vec2d& velocity, double time) const {
    Vec2d thrust = motor_.getThrustVector(time);
    return thrust.dot(velocity);
}

bool PhysicsModel::checkCollision(const Vec2d& position) const {
    if (!scenario_) return false;
    
    int x = static_cast<int>(position.x);
    int y = static_cast<int>(position.y);
    
    return !scenario_->isNavigable(x, y);
}

}
