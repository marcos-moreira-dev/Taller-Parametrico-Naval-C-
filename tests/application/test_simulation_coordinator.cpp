/**
 * @file test_simulation_coordinator.cpp
 * @brief Tests unitarios para SimulationCoordinator
 * @author Proyecto Taller Paramétrico
 * @version 1.0.0
 * 
 * @section TEST_STRATEGY
 * 
 * Este archivo contiene tests unitarios para SimulationCoordinator.
 * Seguimos el patrón Arrange-Act-Assert (AAA):
 * 
 * 1. ARRANGE: Configurar el estado inicial
 * 2. ACT: Ejecutar la acción a probar
 * 3. ASSERT: Verificar el resultado
 * 
 * NOTA SOBRE TIPOS:
 * Hay dos SimulationState en el código (shared y application).
 * Usamos 'auto' en lambdas para evitar ambigüedades.
 */

#include <iostream>
#include <cassert>
#include <cmath>
#include "SimulationCoordinator.hpp"
#include "ExperimentService.hpp"

// Usamos namespace completo para evitar ambigüedades con SimulationState
using namespace tp::domain;

/* ============================================================================
 * TEST HELPERS
 * ============================================================================ */

/**
 * @brief Crea una configuración de experimento mínima para testing
 */
tp::application::ExperimentConfig createTestConfig() {
    tp::application::ExperimentConfig config;
    config.scenario = Scenario(10, 10);
    config.field = VectorField::uniform(0.0, 0.0);
    config.boat = Boat();
    config.boat.setPosition({5.0, 5.0});
    config.motor = Motor();
    config.timeStep = 0.01;
    config.maxTime = 1.0;
    config.method = tp::shared::IntegrationMethod::Euler;
    return config;
}

/**
 * @brief Verifica si dos doubles son aproximadamente iguales
 */
bool approxEqual(double a, double b, double epsilon = 1e-9) {
    return std::abs(a - b) < epsilon;
}

/* ============================================================================
 * TESTS DE CICLO DE VIDA BÁSICO
 * ============================================================================ */

void test_initial_state() {
    std::cout << "Test: Estado inicial..." << std::endl;
    
    tp::application::SimulationCoordinator coordinator;
    
    assert(coordinator.getCurrentState() == tp::application::SimulationState::Stopped);
    assert(!coordinator.isRunning());
    assert(!coordinator.isPaused());
    assert(approxEqual(coordinator.getCurrentTime(), 0.0));
    
    std::cout << "  [PASS] Estado inicial correcto (Stopped)" << std::endl;
}

void test_start_transition() {
    std::cout << "Test: Transición STOPPED → RUNNING..." << std::endl;
    
    tp::application::SimulationCoordinator coordinator;
    coordinator.initialize(createTestConfig());
    coordinator.start();
    
    assert(coordinator.getCurrentState() == tp::application::SimulationState::Running);
    assert(coordinator.isRunning());
    assert(!coordinator.isPaused());
    
    std::cout << "  [PASS] Transición a Running correcta" << std::endl;
}

void test_pause_toggle() {
    std::cout << "Test: Transición RUNNING ↔ PAUSED..." << std::endl;
    
    tp::application::SimulationCoordinator coordinator;
    coordinator.initialize(createTestConfig());
    coordinator.start();
    
    coordinator.pause();
    assert(coordinator.isRunning());
    assert(coordinator.isPaused());
    
    coordinator.pause();
    assert(coordinator.isRunning());
    assert(!coordinator.isPaused());
    
    std::cout << "  [PASS] Pausa/reanudación funciona correctamente" << std::endl;
}

void test_stop_transition() {
    std::cout << "Test: Transición RUNNING → STOPPED..." << std::endl;
    
    tp::application::SimulationCoordinator coordinator;
    coordinator.initialize(createTestConfig());
    coordinator.start();
    coordinator.stop();
    
    assert(coordinator.getCurrentState() == tp::application::SimulationState::Stopped);
    assert(!coordinator.isRunning());
    assert(!coordinator.isPaused());
    
    std::cout << "  [PASS] Stop funciona correctamente" << std::endl;
}

void test_reset() {
    std::cout << "Test: Reset completo..." << std::endl;
    
    tp::application::SimulationCoordinator coordinator;
    coordinator.initialize(createTestConfig());
    coordinator.start();
    coordinator.step();
    coordinator.step();
    coordinator.reset();
    
    assert(coordinator.getCurrentState() == tp::application::SimulationState::Stopped);
    assert(approxEqual(coordinator.getCurrentTime(), 0.0));
    assert(coordinator.getTrajectory().empty());
    
    std::cout << "  [PASS] Reset limpia correctamente" << std::endl;
}

/* ============================================================================
 * TESTS DE CALLBACKS
 * ============================================================================ */

void test_state_callback() {
    std::cout << "Test: Callback de cambio de estado..." << std::endl;
    
    tp::application::SimulationCoordinator coordinator;
    coordinator.initialize(createTestConfig());
    
    bool callbackInvoked = false;
    tp::application::SimulationState capturedState = tp::application::SimulationState::Stopped;
    
    // Usamos 'auto' en el parámetro para evitar ambigüedad
    coordinator.registerStateCallback([&](auto state) {
        callbackInvoked = true;
        capturedState = state;
    });
    
    coordinator.start();
    
    assert(callbackInvoked);
    assert(capturedState == tp::application::SimulationState::Running);
    
    std::cout << "  [PASS] Callback de estado invocado correctamente" << std::endl;
}

void test_multiple_callbacks() {
    std::cout << "Test: Múltiples callbacks..." << std::endl;
    
    tp::application::SimulationCoordinator coordinator;
    coordinator.initialize(createTestConfig());
    
    int callbackCount = 0;
    
    coordinator.registerStateCallback([&](auto) { callbackCount++; });
    coordinator.registerStateCallback([&](auto) { callbackCount++; });
    
    coordinator.start();
    
    assert(callbackCount == 2);
    
    std::cout << "  [PASS] Múltiples callbacks invocados" << std::endl;
}

/* ============================================================================
 * TESTS DE MÉTRICAS
 * ============================================================================ */

void test_initial_statistics() {
    std::cout << "Test: Estadísticas iniciales..." << std::endl;
    
    tp::application::SimulationCoordinator coordinator;
    coordinator.initialize(createTestConfig());
    
    auto stats = coordinator.getStatistics();
    
    assert(approxEqual(stats.totalTime, 0.0));
    assert(approxEqual(stats.totalDistance, 0.0));
    assert(approxEqual(stats.maxSpeed, 0.0));
    assert(approxEqual(stats.netDisplacement, 0.0));
    
    std::cout << "  [PASS] Estadísticas iniciales son cero" << std::endl;
}

void test_trajectory_accumulation() {
    std::cout << "Test: Acumulación de trayectoria..." << std::endl;
    
    tp::application::SimulationCoordinator coordinator;
    coordinator.initialize(createTestConfig());
    coordinator.start();
    
    coordinator.step();
    coordinator.step();
    coordinator.step();
    
    assert(coordinator.getTrajectory().size() == 3);
    
    std::cout << "  [PASS] Trayectoria acumula " << coordinator.getTrajectory().size() 
              << " puntos" << std::endl;
}

/* ============================================================================
 * TESTS DE EDGE CASES
 * ============================================================================ */

void test_step_without_initialize() {
    std::cout << "Test: Step sin inicializar..." << std::endl;
    
    tp::application::SimulationCoordinator coordinator;
    // No llamamos initialize()
    
    bool result = coordinator.step();
    
    assert(!result);
    assert(coordinator.getCurrentState() == tp::application::SimulationState::Stopped);
    
    std::cout << "  [PASS] Step sin inicializar retorna false" << std::endl;
}

void test_start_without_initialize() {
    std::cout << "Test: Start sin inicializar..." << std::endl;
    
    tp::application::SimulationCoordinator coordinator;
    coordinator.start();
    
    // El comportamiento puede variar, pero no debería crash
    std::cout << "  [PASS] Start sin inicializar manejado" << std::endl;
}

void test_double_start() {
    std::cout << "Test: Doble start (idempotencia)..." << std::endl;
    
    tp::application::SimulationCoordinator coordinator;
    coordinator.initialize(createTestConfig());
    
    int callbackCount = 0;
    coordinator.registerStateCallback([&](auto) { callbackCount++; });
    
    coordinator.start();
    coordinator.start();
    
    assert(callbackCount == 1);
    assert(coordinator.getCurrentState() == tp::application::SimulationState::Running);
    
    std::cout << "  [PASS] Doble start es idempotente" << std::endl;
}

/* ============================================================================
 * MAIN
 * ============================================================================ */

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "Tests de SimulationCoordinator" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    try {
        test_initial_state();
        test_start_transition();
        test_pause_toggle();
        test_stop_transition();
        test_reset();
        
        std::cout << std::endl;
        
        test_state_callback();
        test_multiple_callbacks();
        
        std::cout << std::endl;
        
        test_initial_statistics();
        test_trajectory_accumulation();
        
        std::cout << std::endl;
        
        test_step_without_initialize();
        test_start_without_initialize();
        test_double_start();
        
        std::cout << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "¡Todos los tests pasaron!" << std::endl;
        std::cout << "========================================" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "ERROR: Excepción desconocida" << std::endl;
        return 1;
    }
}
