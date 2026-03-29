#include <iostream>
#include <cassert>
#include <cmath>
#include "integrators/Integrators.hpp"
#include "models/PhysicsModel.hpp"
#include "Boat.hpp"
#include "VectorField.hpp"
#include "Motor.hpp"

using namespace tp::simulation;
using namespace tp::domain;
using namespace tp::shared;
using namespace tp::domain;
using namespace tp::shared;

void test_euler_integrator() {
    Boat boat;
    boat.setMass(1.0);
    boat.setDragCoefficient(0.0);
    
    VectorField field = VectorField::uniform(0.0, 0.0);
    Motor motor = Motor::constant(1.0, 0.0);
    
    PhysicsModel model(boat, field, motor);
    
    PhysicsState state;
    state.time = 0.0;
    state.position = Vec2d(0.0, 0.0);
    state.velocity = Vec2d(0.0, 0.0);
    state.orientation = 0.0;
    state.kineticEnergy = 0.0;
    state.totalEnergy = 0.0;
    state.accumulatedWork = 0.0;
    state.collision = false;

    EulerIntegrator euler;

    // Step with dt = 0.1
    PhysicsState next = euler.step(state, model, 0.1);
    
    // With constant force 1.0 and mass 1.0, acceleration = 1.0
    // v_new = v_old + a*dt = 0 + 1.0*0.1 = 0.1
    assert(std::abs(next.velocity.x - 0.1) < 0.001);
    
    std::cout << "✓ Euler integrator test passed\n";
}

void test_rk4_integrator() {
    Boat boat;
    boat.setMass(1.0);
    boat.setDragCoefficient(0.0);
    
    VectorField field = VectorField::uniform(0.0, 0.0);
    Motor motor = Motor::constant(1.0, 0.0);
    
    PhysicsModel model(boat, field, motor);
    
    PhysicsState state;
    state.time = 0.0;
    state.position = Vec2d(0.0, 0.0);
    state.velocity = Vec2d(0.0, 0.0);
    state.orientation = 0.0;
    state.kineticEnergy = 0.0;
    state.totalEnergy = 0.0;
    state.accumulatedWork = 0.0;
    state.collision = false;

    RK4Integrator rk4;

    // Step with dt = 0.1
    PhysicsState next = rk4.step(state, model, 0.1);
    
    // RK4 should give similar results to Euler for constant acceleration
    assert(std::abs(next.velocity.x - 0.1) < 0.001);
    
    std::cout << "✓ RK4 integrator test passed\n";
}

int main() {
    std::cout << "Running Integrator Tests...\n\n";
    
    test_euler_integrator();
    test_rk4_integrator();
    
    std::cout << "\nAll integrator tests passed!\n";
    return 0;
}
