#include "Motor.hpp"
#include <cmath>

namespace tp::domain {

Motor::Motor()
    : constantThrust_(0.0)
    , direction_(0.0)
    , useFunction_(false) {}

double Motor::getThrustMagnitude(double time) const {
    if (useFunction_ && thrustFunction_) {
        return thrustFunction_(time);
    }
    return constantThrust_;
}

Vec2d Motor::getThrustVector(double time) const {
    double thrust = getThrustMagnitude(time);
    return Vec2d(
        thrust * std::cos(direction_),
        thrust * std::sin(direction_)
    );
}

void Motor::setConstantThrust(double magnitude) {
    constantThrust_ = magnitude;
    useFunction_ = false;
}

void Motor::setThrustFunction(ThrustFunction func) {
    thrustFunction_ = std::move(func);
    useFunction_ = true;
}

Motor Motor::constant(double magnitude, double direction) {
    Motor m;
    m.setConstantThrust(magnitude);
    m.setDirection(direction);
    return m;
}

Motor Motor::sinusoidal(double baseThrust, double amplitude, double frequency, double direction) {
    Motor m;
    m.setThrustFunction([baseThrust, amplitude, frequency](double time) {
        return baseThrust + amplitude * std::sin(frequency * time);
    });
    m.setDirection(direction);
    return m;
}

}
