#include "SimulationCoordinator.hpp"
#include "Logger.hpp"
#include <algorithm>

namespace tp::application {

SimulationCoordinator::SimulationCoordinator()
    : currentState_(SimulationState::Stopped)
    , isPaused_(false)
    , currentTime_(0.0)
    , timeStep_(0.01)
    , maxTime_(10.0)
    , currentMethod_(IntegrationMethod::Euler) {
    LOG_INFO_MACRO("SimulationCoordinator inicializado");
}

void SimulationCoordinator::initialize(const ExperimentConfig& config) {
    LOG_INFO_MACRO("Inicializando coordinador de simulación...");
    
    config_ = config;
    currentTime_ = 0.0;
    currentState_ = SimulationState::Stopped;
    isPaused_ = false;
    trajectory_.clear();
    
    // Crear runner con la configuración inicial
    runner_ = std::make_unique<simulation::SimulationRunner>(
        config_.boat, config_.field, config_.motor, config_.scenario
    );
    
    runner_->setTimeStep(config_.timeStep);
    runner_->setMaxTime(config_.maxTime);
    
    // Configurar integrador según el método seleccionado
    setIntegrationMethod(config_.method);
    
    LOG_INFO_MACRO("Coordinador inicializado correctamente");
}

void SimulationCoordinator::start() {
    if (currentState_ == SimulationState::Running) {
        LOG_WARNING("La simulación ya está en ejecución");
        return;
    }
    
    if (currentState_ == SimulationState::Finished) {
        reset();
    }
    
    currentState_ = SimulationState::Running;
    isPaused_ = false;
    
    LOG_INFO_MACRO("Simulación iniciada");
    
    // Notificar callbacks
    for (auto& callback : stateCallbacks_) {
        callback(currentState_);
    }
}

void SimulationCoordinator::pause() {
    if (currentState_ != SimulationState::Running) {
        return;
    }
    
    isPaused_ = !isPaused_;
    
    if (isPaused_) {
        LOG_INFO_MACRO("Simulación pausada");
    } else {
        LOG_INFO_MACRO("Simulación reanudada");
    }
}

void SimulationCoordinator::stop() {
    if (currentState_ == SimulationState::Stopped) {
        return;
    }
    
    currentState_ = SimulationState::Stopped;
    isPaused_ = false;
    
    if (runner_) {
        runner_->stop();
    }
    
    LOG_INFO_MACRO("Simulación detenida");
    
    // Notificar callbacks
    for (auto& callback : stateCallbacks_) {
        callback(currentState_);
    }
}

void SimulationCoordinator::reset() {
    stop();
    
    currentTime_ = 0.0;
    trajectory_.clear();
    currentState_ = SimulationState::Stopped;
    isPaused_ = false;
    
    // Reiniciar el bote a su posición inicial
    config_.boat.reset();
    
    if (runner_) {
        runner_->reset();
    }
    
    LOG_INFO_MACRO("Simulación reiniciada");
}

bool SimulationCoordinator::step() {
    if (currentState_ != SimulationState::Running || isPaused_) {
        return false;
    }
    
    if (!runner_) {
        LOG_ERROR_MACRO("Runner no inicializado");
        return false;
    }
    
    // Actualizar configuración en tiempo real
    updateConfiguration();
    
    // Ejecutar un paso de simulación
    auto result = runner_->run();
    
    if (!result.states.empty()) {
        const auto& state = result.states.back();
        currentTime_ = state.time;
        
        // Guardar punto de trayectoria
        TrajectoryPoint point;
        point.x = state.position.x;
        point.y = state.position.y;
        point.time = state.time;
        point.velocity = state.velocity.magnitude();
        point.energy = state.kineticEnergy;
        trajectory_.push_back(point);
        
        // Actualizar posición del bote
        config_.boat.setPosition(state.position);
        config_.boat.setVelocity(state.velocity);
        
        // Verificar colisión
        if (state.collision) {
            currentState_ = SimulationState::Finished;
            LOG_INFO_MACRO("Simulación finalizada por colisión");
            
            for (auto& callback : stateCallbacks_) {
                callback(currentState_);
            }
            return false;
        }
        
        // Verificar tiempo máximo
        if (currentTime_ >= maxTime_) {
            currentState_ = SimulationState::Finished;
            LOG_INFO_MACRO("Simulación finalizada (tiempo máximo alcanzado)");
            
            for (auto& callback : stateCallbacks_) {
                callback(currentState_);
            }
            return false;
        }
        
        // Notificar actualización
        for (auto& callback : updateCallbacks_) {
            callback(state);
        }
        
        return true;
    }
    
    return false;
}

void SimulationCoordinator::runComplete() {
    if (currentState_ != SimulationState::Running) {
        start();
    }
    
    LOG_INFO_MACRO("Ejecutando simulación completa...");
    
    // Ejecutar hasta que termine
    while (step()) {
        // Continuar hasta colisión o tiempo máximo
    }
    
    LOG_INFO_MACRO("Simulación completa finalizada");
}

void SimulationCoordinator::setIntegrationMethod(IntegrationMethod method) {
    currentMethod_ = method;
    
    if (!runner_) {
        return;
    }
    
    std::unique_ptr<simulation::Integrator> integrator;
    
    switch (method) {
        case IntegrationMethod::Euler:
            LOG_DEBUG_MACRO("Usando integrador Euler");
            integrator = std::make_unique<simulation::EulerIntegrator>();
            break;
            
        case IntegrationMethod::ImprovedEuler:
            LOG_DEBUG_MACRO("Usando integrador Euler Mejorado");
            integrator = std::make_unique<simulation::ImprovedEulerIntegrator>();
            break;
            
        case IntegrationMethod::RK4:
            LOG_DEBUG_MACRO("Usando integrador RK4");
            integrator = std::make_unique<simulation::RK4Integrator>();
            break;
            
        default:
            LOG_WARNING("Método desconocido, usando Euler");
            integrator = std::make_unique<simulation::EulerIntegrator>();
    }
    
    runner_->setIntegrator(std::move(integrator));
}

void SimulationCoordinator::updateConfiguration() {
    if (!runner_) {
        return;
    }
    
    // Actualizar parámetros que pueden cambiar en tiempo real
    runner_->setTimeStep(config_.timeStep);
    runner_->setMaxTime(currentTime_ + config_.timeStep + 0.001);
}

SimulationStats SimulationCoordinator::getStatistics() const {
    SimulationStats stats;
    stats.totalTime = currentTime_;
    stats.totalDistance = 0.0;
    stats.maxSpeed = 0.0;
    stats.averageSpeed = 0.0;
    stats.finalEnergy = 0.0;
    
    if (trajectory_.empty()) {
        return stats;
    }
    
    // Calcular estadísticas de la trayectoria
    double totalDistance = 0.0;
    double maxSpeed = 0.0;
    double totalEnergy = 0.0;
    
    for (size_t i = 1; i < trajectory_.size(); ++i) {
        double dx = trajectory_[i].x - trajectory_[i-1].x;
        double dy = trajectory_[i].y - trajectory_[i-1].y;
        totalDistance += std::sqrt(dx*dx + dy*dy);
        
        if (trajectory_[i].velocity > maxSpeed) {
            maxSpeed = trajectory_[i].velocity;
        }
        
        totalEnergy += trajectory_[i].energy;
    }
    
    stats.totalDistance = totalDistance;
    stats.maxSpeed = maxSpeed;
    stats.averageSpeed = trajectory_.size() > 1 ? 
        totalDistance / currentTime_ : 0.0;
    stats.finalEnergy = trajectory_.back().energy;
    
    return stats;
}

void SimulationCoordinator::registerStateCallback(StateCallback callback) {
    stateCallbacks_.push_back(std::move(callback));
}

void SimulationCoordinator::registerUpdateCallback(UpdateCallback callback) {
    updateCallbacks_.push_back(std::move(callback));
}

} // namespace tp::application
