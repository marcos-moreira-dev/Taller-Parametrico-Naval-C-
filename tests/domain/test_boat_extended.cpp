/**
 * @file test_boat_extended.cpp
 * @brief Tests extendidos para la clase Boat
 * @details Tests de conservación de energía, colisiones y límites
 */

#include <iostream>
#include <cassert>
#include <cmath>
#include "Boat.hpp"

using namespace tp::domain;

bool approxEqual(double a, double b, double epsilon = 1e-6) {
    return std::abs(a - b) < epsilon;
}

/* ============================================================================
 * TESTS DE CONSERVACIÓN DE ENERGÍA
 * ============================================================================ */

/**
 * @test Conservación de energía con fuerza perpendicular
 * @details Una fuerza perpendicular a la velocidad no cambia la magnitud,
 *          solo la dirección (como la fuerza centrípeta)
 */
void test_energy_conservation_perpendicular_force() {
    std::cout << "Test: Conservación de energía (fuerza perpendicular)..." << std::endl;
    
    Boat boat;
    boat.setMass(10.0);
    boat.setVelocity({5.0, 0.0});  // 5 m/s en X
    
    double initialSpeed = boat.getVelocity().magnitude();
    double initialEnergy = 0.5 * boat.getMass() * initialSpeed * initialSpeed;
    
    // Aplicar fuerza perpendicular (en Y)
    for (int i = 0; i < 100; ++i) {
        boat.applyForce({0.0, 10.0}, 0.001);
    }
    
    double finalSpeed = boat.getVelocity().magnitude();
    double finalEnergy = 0.5 * boat.getMass() * finalSpeed * finalSpeed;
    
    // La energía cinética debe conservarse (aproximadamente)
    assert(approxEqual(initialEnergy, finalEnergy, 0.1));
    
    std::cout << "  [PASS] Energía conservada: " << initialEnergy << " -> " << finalEnergy << std::endl;
}

/**
 * @test Aumento de energía con fuerza en dirección del movimiento
 */
void test_energy_increase_with_parallel_force() {
    std::cout << "Test: Aumento de energía (fuerza paralela)..." << std::endl;
    
    Boat boat;
    boat.setMass(10.0);
    boat.setVelocity({5.0, 0.0});
    
    double initialEnergy = 0.5 * boat.getMass() * 25.0;  // 1/2 * 10 * 25 = 125 J
    
    // Aplicar fuerza en la misma dirección
    for (int i = 0; i < 100; ++i) {
        boat.applyForce({10.0, 0.0}, 0.001);
    }
    
    double finalSpeed = boat.getVelocity().magnitude();
    double finalEnergy = 0.5 * boat.getMass() * finalSpeed * finalSpeed;
    
    // La energía debe aumentar
    assert(finalEnergy > initialEnergy);
    
    std::cout << "  [PASS] Energía aumentó: " << initialEnergy << " -> " << finalEnergy << std::endl;
}

/* ============================================================================
 * TESTS DE LÍMITES Y COLISIONES
 * ============================================================================ */

/**
 * @test Velocidad máxima (drag cuadrático limita la velocidad)
 */
void test_terminal_velocity() {
    std::cout << "Test: Velocidad terminal..." << std::endl;
    
    Boat boat;
    boat.setMass(10.0);
    boat.setDragCoefficient(0.5);
    boat.setSize(2.0);
    boat.setVelocity({0.0, 0.0});
    
    // Aplicar fuerza constante
    for (int i = 0; i < 1000; ++i) {
        boat.applyForce({100.0, 0.0}, 0.01);
    }
    
    double speed1 = boat.getVelocity().magnitude();
    
    // Continuar aplicando fuerza
    for (int i = 0; i < 1000; ++i) {
        boat.applyForce({100.0, 0.0}, 0.01);
    }
    
    double speed2 = boat.getVelocity().magnitude();
    
    // La velocidad debe estabilizarse (velocidad terminal)
    // La diferencia entre speed1 y speed2 debe ser pequeña
    assert(std::abs(speed2 - speed1) < 1.0);
    
    std::cout << "  [PASS] Velocidad terminal alcanzada: " << speed2 << " m/s" << std::endl;
}

/**
 * @test Reset del bote
 */
void test_boat_reset() {
    std::cout << "Test: Reset de bote..." << std::endl;
    
    Boat boat;
    boat.setPosition({10.0, 20.0});
    boat.setVelocity({5.0, 3.0});
    boat.setOrientation(1.5);
    
    boat.reset();
    
    auto pos = boat.getPosition();
    auto vel = boat.getVelocity();
    
    assert(approxEqual(pos.x, 0.0));
    assert(approxEqual(pos.y, 0.0));
    assert(approxEqual(vel.x, 0.0));
    assert(approxEqual(vel.y, 0.0));
    assert(approxEqual(boat.getOrientation(), 0.0));
    
    std::cout << "  [PASS] Bote reseteado correctamente" << std::endl;
}

/* ============================================================================
 * TESTS DE ROTACIÓN Y TORQUE
 * ============================================================================ */

/**
 * @test Aplicación de torque
 * @note El momento de inercia se calcula automáticamente a partir de la masa
 */
void test_torque_application() {
    std::cout << "Test: Aplicación de torque..." << std::endl;
    
    Boat boat;
    boat.setMass(10.0);  // Esto actualiza el momento de inercia automáticamente
    boat.setAngularVelocity(0.0);
    
    // Aplicar torque
    for (int i = 0; i < 100; ++i) {
        boat.applyTorque(5.0, 0.001);
    }
    
    // Debe haber rotación
    assert(boat.getAngularVelocity() != 0.0);
    assert(boat.getOrientation() != 0.0);
    
    std::cout << "  [PASS] Torque aplicado, orientación: " << boat.getOrientation() << std::endl;
}

/* ============================================================================
 * MAIN
 * ============================================================================ */

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "Tests Extendidos de Boat" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    try {
        test_energy_conservation_perpendicular_force();
        test_energy_increase_with_parallel_force();
        test_terminal_velocity();
        test_boat_reset();
        test_torque_application();
        
        std::cout << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "¡Todos los tests extendidos pasaron!" << std::endl;
        std::cout << "========================================" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}
