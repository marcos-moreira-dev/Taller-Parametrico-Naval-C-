#include "SimulationRunner.hpp"
#include <cmath>
#include <limits>

namespace tp::simulation {

SimulationRunner::SimulationRunner(const Boat& boat, const VectorField& field, 
                                   const Motor& motor, const Scenario& scenario)
    : boat_(boat)
    , field_(field)
    , motor_(motor)
    , scenario_(scenario)
    , timeStep_(0.01)
    , maxTime_(10.0)
    , running_(false)
    , paused_(false)
    , stopped_(false) {}

void SimulationRunner::setIntegrator(std::unique_ptr<Integrator> integrator) {
    integrator_ = std::move(integrator);
}

void SimulationRunner::reset() {
    running_ = false;
    paused_ = false;
    stopped_ = false;
}

SimulationResult SimulationRunner::run() {
    SimulationResult result;
    result.methodUsed = IntegrationMethod::Euler;
    
    if (!integrator_) {
        return result;
    }
    
    running_ = true;
    stopped_ = false;
    paused_ = false;
    
    PhysicsModel model(boat_, field_, motor_);
    model.setScenario(&scenario_);
    
    PhysicsState state;
    state.time = 0.0;
    state.position = boat_.getPosition();
    state.velocity = boat_.getVelocity();
    state.orientation = boat_.getOrientation();
    state.kineticEnergy = 0.0;
    state.totalEnergy = 0.0;
    state.accumulatedWork = 0.0;
    state.collision = false;
    
    result.states.push_back(state);
    
    double totalDistance = 0.0;
    double maxSpeed = 0.0;
    
    while (state.time < maxTime_ && !stopped_) {
        if (paused_) {
            continue;
        }
        
        if (state.collision) {
            break;
        }
        
        PhysicsState prevState = state;
        state = integrator_->step(state, model, timeStep_);
        
        double stepDistance = (state.position - prevState.position).magnitude();
        totalDistance += stepDistance;
        
        double speed = state.velocity.magnitude();
        if (speed > maxSpeed) {
            maxSpeed = speed;
        }
        
        result.states.push_back(state);
    }
    
    running_ = false;
    
    result.finalTime = state.time;
    result.totalWork = state.accumulatedWork;
    result.maxSpeed = maxSpeed;
    result.totalDistance = totalDistance;
    result.endedByCollision = state.collision;
    
    return result;
}

}
