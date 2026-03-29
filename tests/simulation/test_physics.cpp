#include <iostream>
#include <cassert>
#include <cmath>
#include "models/PhysicsModel.hpp"
#include "Boat.hpp"
#include "VectorField.hpp"
#include "Motor.hpp"

using namespace tp::simulation;
using namespace tp::domain;
using namespace tp::shared;
using namespace tp::shared;

void test_acceleration() {
    Boat boat;
    boat.setMass(1.0);
    boat.setDragCoefficient(0.0);
    
    VectorField field = VectorField::uniform(0.0, 0.0);
    Motor motor = Motor::constant(1.0, 0.0);
    
    PhysicsModel model(boat, field, motor);
    
    // With no velocity and constant thrust of 1.0, acceleration should be 1.0
    Vec2d accel = model.computeAcceleration(Vec2d(0.0, 0.0), Vec2d(0.0, 0.0), 0.0);
    assert(std::abs(accel.x - 1.0) < 0.001);
    assert(std::abs(accel.y - 0.0) < 0.001);
    
    std::cout << "✓ Acceleration test passed\n";
}

void test_drag() {
    Boat boat;
    boat.setMass(1.0);
    boat.setDragCoefficient(0.5);
    
    VectorField field = VectorField::uniform(0.0, 0.0);
    Motor motor = Motor::constant(0.0, 0.0);
    
    PhysicsModel model(boat, field, motor);
    
    // With velocity 1.0 and drag coefficient 0.5, drag should be -0.5
    Vec2d accel = model.computeAcceleration(Vec2d(0.0, 0.0), Vec2d(1.0, 0.0), 0.0);
    assert(std::abs(accel.x - (-0.5)) < 0.001);
    
    std::cout << "✓ Drag test passed\n";
}

void test_power() {
    Boat boat;
    Motor motor = Motor::constant(2.0, 0.0);
    VectorField field = VectorField::uniform(0.0, 0.0);
    
    PhysicsModel model(boat, field, motor);
    
    // Power = F · v = 2.0 * 3.0 = 6.0
    double power = model.computePower(Vec2d(3.0, 0.0), 0.0);
    assert(std::abs(power - 6.0) < 0.001);
    
    std::cout << "✓ Power test passed\n";
}

int main() {
    std::cout << "Running Physics Tests...\n\n";
    
    test_acceleration();
    test_drag();
    test_power();
    
    std::cout << "\nAll physics tests passed!\n";
    return 0;
}
