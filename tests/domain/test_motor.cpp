#include <iostream>
#include <cassert>
#include <cmath>
#include "Motor.hpp"

using namespace tp::domain;
using namespace tp::shared;

void test_motor_constant() {
    auto motor = Motor::constant(5.0, 0.0);
    
    assert(motor.getThrustMagnitude() == 5.0);
    assert(std::abs(motor.getDirection() - 0.0) < 0.001);
    
    std::cout << "✓ Motor constant test passed\n";
}

void test_motor_thrust() {
    auto motor = Motor::constant(3.0, 1.57);
    
    Vec2d thrust = motor.getThrust(0.0);
    assert(std::abs(thrust.x - 0.0) < 0.001);
    assert(std::abs(thrust.y - 3.0) < 0.001);
    
    std::cout << "✓ Motor thrust calculation test passed\n";
}

int main() {
    std::cout << "Running Motor Tests...\n\n";
    
    test_motor_constant();
    test_motor_thrust();
    
    std::cout << "\nAll motor tests passed!\n";
    return 0;
}