#include "Integrators.hpp"

namespace tp::simulation {

PhysicsState EulerIntegrator::step(const PhysicsState& current, const PhysicsModel& model,
                                       double dt) {
    PhysicsState next = current;
    
    if (current.collision) {
        return next;
    }
    
    Vec2d accel = model.computeAcceleration(current.position, current.velocity, current.time);
    
    next.position = current.position + current.velocity * dt;
    next.velocity = current.velocity + accel * dt;
    next.time = current.time + dt;
    
    double power = model.computePower(next.velocity, next.time);
    next.accumulatedWork += power * dt;
    next.kineticEnergy = 0.5 * next.velocity.magnitudeSquared();
    next.totalEnergy = next.kineticEnergy;
    
    next.collision = model.checkCollision(next.position);
    
    return next;
}

PhysicsState ImprovedEulerIntegrator::step(const PhysicsState& current,
                                                const PhysicsModel& model, double dt) {
    PhysicsState next = current;
    
    if (current.collision) {
        return next;
    }
    
    Vec2d accel1 = model.computeAcceleration(current.position, current.velocity, current.time);
    
    Vec2d pos2 = current.position + current.velocity * dt;
    Vec2d vel2 = current.velocity + accel1 * dt;
    double time2 = current.time + dt;
    
    Vec2d accel2 = model.computeAcceleration(pos2, vel2, time2);
    
    next.position = current.position + (current.velocity + vel2) * (dt * 0.5);
    next.velocity = current.velocity + (accel1 + accel2) * (dt * 0.5);
    next.time = time2;
    
    double power = model.computePower(next.velocity, next.time);
    next.accumulatedWork += power * dt;
    next.kineticEnergy = 0.5 * next.velocity.magnitudeSquared();
    next.totalEnergy = next.kineticEnergy;
    
    next.collision = model.checkCollision(next.position);
    
    return next;
}

PhysicsState RK4Integrator::step(const PhysicsState& current, const PhysicsModel& model,
                                     double dt) {
    PhysicsState next = current;
    
    if (current.collision) {
        return next;
    }
    
    double t = current.time;
    Vec2d p = current.position;
    Vec2d v = current.velocity;
    
    Vec2d a1 = model.computeAcceleration(p, v, t);
    Vec2d k1_v = a1 * dt;
    Vec2d k1_p = v * dt;
    
    Vec2d a2 = model.computeAcceleration(p + k1_p * 0.5, v + k1_v * 0.5, t + dt * 0.5);
    Vec2d k2_v = a2 * dt;
    Vec2d k2_p = (v + k1_v * 0.5) * dt;
    
    Vec2d a3 = model.computeAcceleration(p + k2_p * 0.5, v + k2_v * 0.5, t + dt * 0.5);
    Vec2d k3_v = a3 * dt;
    Vec2d k3_p = (v + k2_v * 0.5) * dt;
    
    Vec2d a4 = model.computeAcceleration(p + k3_p, v + k3_v, t + dt);
    Vec2d k4_v = a4 * dt;
    Vec2d k4_p = (v + k3_v) * dt;
    
    next.position = p + (k1_p + k2_p * 2.0 + k3_p * 2.0 + k4_p) * (1.0 / 6.0);
    next.velocity = v + (k1_v + k2_v * 2.0 + k3_v * 2.0 + k4_v) * (1.0 / 6.0);
    next.time = t + dt;
    
    double power = model.computePower(next.velocity, next.time);
    next.accumulatedWork += power * dt;
    next.kineticEnergy = 0.5 * next.velocity.magnitudeSquared();
    next.totalEnergy = next.kineticEnergy;
    
    next.collision = model.checkCollision(next.position);
    
    return next;
}

}
