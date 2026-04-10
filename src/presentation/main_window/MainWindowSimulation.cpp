#include "main_window/MainWindow.hpp"

#include "main_window/MainWindowIds.hpp"
#include "main_window/MainWindowStyles.hpp"
#include "panels/BottomPanel.hpp"
#include "panels/LeftPanel.hpp"
#include "panels/RightPanel.hpp"
#include "panels/TheoryPanel.hpp"
#include "panels/EditorPanel.hpp"
#include "panels/FieldEditorPanel.hpp"
#include "widgets/SimulationCanvas.hpp"
#include "widgets/Water3DCanvas.hpp"
#include "widgets/SimpleGraph.hpp"

#include <wx/aboutdlg.h>
#include <wx/artprov.h>
#include <wx/filename.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>

#include <fstream>
#include <iomanip>

#include "Logger.hpp"
#include "TraceLogger.hpp"
#include "ExpressionParser.hpp"
#include "Scenario.hpp"
#include "VectorField.hpp"
#include "MathExpression.hpp"
#include "Boat.hpp"
#include "Motor.hpp"
#include "integrators/Integrators.hpp"
#include "models/PhysicsModel.hpp"
#include "runner/SimulationRunner.hpp"

namespace tp::presentation {

void MainWindow::onStartSimulation(wxCommandEvent& event)
{
    (void)event;
    TRACE_INFO(tp::shared::TraceEventType::STATE_CHANGE, "Simulation", "onStartSimulation");
    if (!isRunning_) {
        auto& config = experimentService_.getConfig();
        trajectoryPoints_.clear();
        if (canvas_) {
            canvas_->clearTrajectory();
        }
        if (canvas3D_) {
            canvas3D_->clearTrajectory();
        }
        if (bottomPanel_) {
            bottomPanel_->clearGraphs();
        }
        currentTime_ = 0.0;
        currentSpeed_ = 0.0;
        currentEnergy_ = 0.0;
        totalDistance_ = 0.0;
        netDisplacement_ = 0.0;
        routeEfficiency_ = 1.0;
        simulationStartPosition_ = config.boat.getPosition();
        simulationStartVelocity_ = config.boat.getVelocity();
        simulationStartOrientation_ = config.boat.getOrientation();
        initialPosition_ = simulationStartPosition_;
        isRunning_ = true;
        isPaused_ = false;
        // Obtener paso temporal del slider (conversión de segundos a ms)
        int dtMs = static_cast<int>(getTimeStep() * 1000.0);
        timer_->Start(dtMs);
        simulationStatusText_ = wxT("Simulación en curso");
        refreshStatusBar();
        if (bottomPanel_) {
            bottomPanel_->addEvent(wxT("Simulación iniciada"));
        }
    }
}

void MainWindow::onPauseSimulation(wxCommandEvent& event)
{
    (void)event;
    TRACE_INFO(tp::shared::TraceEventType::STATE_CHANGE, "Simulation", "onPauseSimulation");
    if (isRunning_) {
        isPaused_ = !isPaused_;
        if (isPaused_) {
            timer_->Stop();
            simulationStatusText_ = wxT("Simulación pausada");
        } else {
            timer_->Start(33);
            simulationStatusText_ = wxT("Simulación en curso");
        }
        refreshStatusBar();
    }
}

void MainWindow::onResetSimulation(wxCommandEvent& event)
{
    (void)event;
    TRACE_INFO(tp::shared::TraceEventType::STATE_CHANGE, "Simulation", "onResetSimulation");
    resetSimulation();
}

void MainWindow::onStepSimulation(wxCommandEvent& event)
{
    (void)event;
    TRACE_INFO(tp::shared::TraceEventType::SIMULATION_STEP, "Simulation", "onStepSimulation");
    if (isRunning_ && isPaused_) {
        runSimulationStep();
    }
}

void MainWindow::onMethodChange(wxCommandEvent& event)
{
    (void)event;
    refreshStatusBar();
}

void MainWindow::onDtChange(wxCommandEvent& event)
{
    (void)event;
    if (timer_ && timer_->IsRunning() && rightPanel_) {
        int dtMs = static_cast<int>(rightPanel_->getTimeStep() * 1000.0);
        timer_->Start(dtMs);
    }
    refreshStatusBar();
}

void MainWindow::onTimer(wxTimerEvent& event)
{
    (void)event;
    runSimulationStep();
}

void MainWindow::runSimulationStep()
{
    if (!isRunning_ || isPaused_) return;
    
    auto& config = experimentService_.getConfig();
    
    // ACTUALIZAR VALORES EN TIEMPO REAL desde los controles del RightPanel
    if (rightPanel_) {
        // Leer valores actuales de los sliders
        double thrust = rightPanel_->getThrustValue();
        double direction = rightPanel_->getDirectionValue();
        
        // Si está activado el empuje automático, calcular dirección óptima
        if (rightPanel_->isAutoThrustEnabled()) {
            // El bote apunta automáticamente hacia la dirección del campo
            // para maximizar la velocidad resultante
            Vec2d fieldVec = config.field.getValue(config.boat.getPosition().x, 
                                                    config.boat.getPosition().y);
            if (fieldVec.magnitude() > 0.1) {
                // Calcular ángulo del campo y añadir un offset para navegar eficientemente
                double fieldAngle = std::atan2(fieldVec.y, fieldVec.x);
                // El bote navega a 30 grados de la corriente para avanzar
                direction = fieldAngle; // Navegar con la corriente
            }
        }
        
        // Actualizar motor en tiempo real
        config.motor = tp::domain::Motor::constant(thrust, direction);
        
        // Actualizar método numérico en tiempo real
        int method = rightPanel_->getMethod();
        switch (method) {
            case 0: config.method = tp::shared::IntegrationMethod::Euler; break;
            case 1: config.method = tp::shared::IntegrationMethod::ImprovedEuler; break;
            case 2: config.method = tp::shared::IntegrationMethod::RK4; break;
        }
        
        // Actualizar paso temporal en tiempo real
        config.timeStep = rightPanel_->getTimeStep();
    }
    
    // Usar el paso temporal actualizado
    double dt = config.timeStep > 0 ? config.timeStep : 0.033;

    tp::simulation::PhysicsModel model(config.boat, config.field, config.motor);
    model.setScenario(&config.scenario);

    std::unique_ptr<tp::simulation::Integrator> integrator;
    switch (config.method) {
        case tp::shared::IntegrationMethod::ImprovedEuler:
            integrator = std::make_unique<tp::simulation::ImprovedEulerIntegrator>();
            break;
        case tp::shared::IntegrationMethod::RK4:
            integrator = std::make_unique<tp::simulation::RK4Integrator>();
            break;
        case tp::shared::IntegrationMethod::Euler:
        default:
            integrator = std::make_unique<tp::simulation::EulerIntegrator>();
            break;
    }

    tp::simulation::PhysicsState currentState;
    currentState.time = currentTime_;
    currentState.position = config.boat.getPosition();
    currentState.velocity = config.boat.getVelocity();
    currentState.orientation = config.boat.getOrientation();
    currentState.kineticEnergy = currentEnergy_;
    currentState.totalEnergy = currentEnergy_;
    currentState.accumulatedWork = 0.0;
    currentState.collision = false;

    const auto state = integrator->step(currentState, model, dt);
    {
        const double stepDistance = (state.position - currentState.position).magnitude();
        
        // Actualizar datos de simulación
        currentTime_ = state.time;
        currentSpeed_ = state.velocity.magnitude();
        currentEnergy_ = state.kineticEnergy;
        totalDistance_ += stepDistance;
        config.boat.setPosition(state.position);
        config.boat.setVelocity(state.velocity);
        config.boat.setOrientation(state.orientation);
        
        // Calcular desplazamiento neto (distancia en línea recta desde el inicio)
        double dx = state.position.x - initialPosition_.x;
        double dy = state.position.y - initialPosition_.y;
        netDisplacement_ = std::sqrt(dx * dx + dy * dy);
        
        // Calcular eficiencia de ruta (desplazamiento neto / distancia total)
        if (totalDistance_ > 0.01) {
            routeEfficiency_ = netDisplacement_ / totalDistance_;
        } else {
            routeEfficiency_ = 1.0;
        }
        
        // Actualizar posición del bote en el canvas
        if (canvas_) {
            canvas_->updateBoatPosition(state.position.x, state.position.y);
            
            // Agregar punto a la trayectoria
            TrajectoryPoint point;
            point.x = state.position.x;
            point.y = state.position.y;
            point.time = state.time;
            trajectoryPoints_.push_back(point);
            canvas_->setTrajectory(trajectoryPoints_);
            
            // Sistema de partículas: emitir estela del bote
            auto* particleSystem = canvas_->getParticleSystem();
            if (particleSystem) {
                Vec2d boatVel = state.velocity;
                double speed = currentSpeed_;
                
                // Emitir estela (wake) detrás del bote
                particleSystem->emitWake(state.position, boatVel, speed);
                
                // Emitir espuma alrededor del bote si hay velocidad
                if (speed > 1.0) {
                    particleSystem->emitFoam(state.position, speed * 0.2, 2);
                }
                
                // Actualizar sistema de partículas
                particleSystem->update(dt);
                
                // Aplicar efecto del campo vectorial (turbulencia)
                Vec2d fieldVec = config.field.getValue(state.position.x, state.position.y);
                if (fieldVec.magnitude() > 2.0) {
                    particleSystem->applyFieldEffect(state.position, fieldVec);
                }
            }
        }

        if (canvas3D_ && is3DMode_) {
            canvas3D_->setVectorField(&config.field);
            canvas3D_->setBoat(&config.boat);
            canvas3D_->setBoatPosition(state.position.x, state.position.y);
            canvas3D_->setSimulationTime(state.time);
            canvas3D_->setTrajectory(trajectoryPoints_);
            canvas3D_->refreshCanvas();
        }
        
        // Actualizar barra de estado con coordenadas
        wxString coords;
        coords.Printf(wxT("Coords: (%.1f, %.1f)"), state.position.x, state.position.y);
        refreshStatusBar();
        
        // Verificar si se alcanzó el tiempo máximo
        if (currentTime_ >= config.maxTime) {
            timer_->Stop();
            isRunning_ = false;
            simulationStatusText_ = wxT("Simulación completada");
            refreshStatusBar();
            if (bottomPanel_) {
                bottomPanel_->addEvent(wxString::Format(
                    wxT("Simulación finalizada - Tiempo: %.2f s, Distancia: %.2f m"),
                    currentTime_, totalDistance_
                ));
            }
        }
        
        // Verificar colisión
        if (state.collision) {
            timer_->Stop();
            isRunning_ = false;
            simulationStatusText_ = wxT("Colisión detectada");
            refreshStatusBar();
            
            if (bottomPanel_) {
                bottomPanel_->addEvent(wxT("¡Colisión detectada!"));
            }
            
            if (canvas_) {
                auto* particleSystem = canvas_->getParticleSystem();
                if (particleSystem) {
                    particleSystem->emitSplash(state.position, state.velocity, 20);
                }
            }
            
            wxMessageBox(wxT("¡Colisión detectada!"), wxT("Simulación"), wxICON_WARNING);
        }
    }
    
    // Actualizar UI de resultados
    updateResults();
    updateCanvas();
}

void MainWindow::resetSimulation()
{
    if (timer_) timer_->Stop();
    isRunning_ = false;
    isPaused_ = false;
    
    currentTime_ = 0.0;
    currentSpeed_ = 0.0;
    currentEnergy_ = 0.0;
    totalDistance_ = 0.0;
    netDisplacement_ = 0.0;
    routeEfficiency_ = 1.0;
    
    // Limpiar trayectoria acumulada
    trajectoryPoints_.clear();
    
    // Restaurar bote al estado inicial de la simulación/documento actual
    auto& config = getExperimentService().getConfig();
    config.boat.reset();
    config.boat.setPosition(simulationStartPosition_);
    config.boat.setVelocity(simulationStartVelocity_);
    config.boat.setOrientation(simulationStartOrientation_);
    initialPosition_ = simulationStartPosition_;
    
    // Limpiar trayectoria en canvas
    if (canvas_) {
        canvas_->clearTrajectory();
        canvas_->updateBoatPosition(simulationStartPosition_.x, simulationStartPosition_.y);
    }
    if (canvas3D_) {
        canvas3D_->clearTrajectory();
    }
    if (bottomPanel_) {
        bottomPanel_->clearGraphs();
    }
    
    updateResults();
    updateCanvas();
    simulationStatusText_ = wxT("Simulación reiniciada");
    refreshStatusBar();
}


} // namespace tp::presentation
