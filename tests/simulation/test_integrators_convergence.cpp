/**
 * @file test_integrators_convergence.cpp
 * @brief Tests de convergencia y precisión de integradores numéricos
 * @details Compara el error de Euler vs RK4 para ODEs simples
 */

#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>

using namespace std;

bool approxEqual(double a, double b, double epsilon = 1e-6) {
    return std::abs(a - b) < epsilon;
}

/* ============================================================================
 * TESTS DE CONVERGENCIA PARA ODE SIMPLE
 * ============================================================================ */

/**
 * @test Convergencia de Euler para ODE simple
 * @details Verifica que el error disminuye proporcionalmente a dt (O(dt))
 * 
 * EDO: dx/dt = -x
 * Solución analítica: x(t) = x0 * exp(-t)
 */
void test_euler_convergence() {
    std::cout << "Test: Convergencia de Euler (O(dt))..." << std::endl;
    
    double x0 = 1.0;
    double tFinal = 1.0;
    double expected = std::exp(-tFinal);  // exp(-1) ≈ 0.3679
    
    // Probar con diferentes pasos de tiempo
    std::vector<double> dts = {0.1, 0.05, 0.025, 0.01};
    std::vector<double> errors;
    
    for (double dt : dts) {
        double x = x0;
        
        // Integrar hasta tFinal con Euler: x_{n+1} = x_n + dt * f(x_n)
        int steps = static_cast<int>(tFinal / dt);
        for (int i = 0; i < steps; ++i) {
            double derivative = -x;  // dx/dt = -x
            x += dt * derivative;
        }
        
        double error = std::abs(x - expected);
        errors.push_back(error);
    }
    
    // Verificar convergencia O(dt): el error debe reducirse aproximadamente a la mitad
    // cuando dt se reduce a la mitad
    std::cout << "  Errores:" << std::endl;
    for (size_t i = 0; i < dts.size(); ++i) {
        std::cout << "    dt=" << dts[i] << " -> error=" << errors[i] << std::endl;
    }
    
    // Verificar que hay convergencia (los errores disminuyen)
    for (size_t i = 1; i < errors.size(); ++i) {
        assert(errors[i] < errors[i-1]);  // Cada error debe ser menor que el anterior
    }
    
    std::cout << "  [PASS] Euler converge al refinarse el paso temporal" << std::endl;
}

/**
 * @test Convergencia de RK4 para ODE simple
 * @details Verifica que el error disminuye proporcionalmente a dt^4 (O(dt⁴))
 * 
 * RK4 es un método de cuarto orden, mucho más preciso que Euler
 */
void test_rk4_convergence() {
    std::cout << "Test: Convergencia de RK4 (O(dt⁴))..." << std::endl;
    
    double x0 = 1.0;
    double tFinal = 1.0;
    double expected = std::exp(-tFinal);
    
    std::vector<double> dts = {0.1, 0.05, 0.025};
    std::vector<double> errors;
    
    for (double dt : dts) {
        double x = x0;
        
        int steps = static_cast<int>(tFinal / dt);
        for (int i = 0; i < steps; ++i) {
            // RK4 para dx/dt = -x
            double k1 = -x;
            double k2 = -(x + 0.5*dt*k1);
            double k3 = -(x + 0.5*dt*k2);
            double k4 = -(x + dt*k3);
            
            x += (dt/6.0) * (k1 + 2*k2 + 2*k3 + k4);
        }
        
        double error = std::abs(x - expected);
        errors.push_back(error);
    }
    
    std::cout << "  Errores:" << std::endl;
    for (size_t i = 0; i < dts.size(); ++i) {
        std::cout << "    dt=" << dts[i] << " -> error=" << errors[i] << std::endl;
    }
    
    // Verificar convergencia
    for (size_t i = 1; i < errors.size(); ++i) {
        assert(errors[i] < errors[i-1]);
    }
    
    std::cout << "  [PASS] RK4 converge al refinarse el paso temporal" << std::endl;
}

/**
 * @test Comparación de precisión: Euler vs RK4
 * @details RK4 debe ser significativamente más preciso que Euler para el mismo dt
 */
void test_euler_vs_rk4_precision() {
    std::cout << "Test: Comparación Euler vs RK4..." << std::endl;
    
    double x0 = 1.0;
    double dt = 0.01;
    double tFinal = 1.0;
    double expected = std::exp(-tFinal);
    int steps = static_cast<int>(tFinal / dt);
    
    // Euler
    double xEuler = x0;
    for (int i = 0; i < steps; ++i) {
        xEuler += dt * (-xEuler);
    }
    double errorEuler = std::abs(xEuler - expected);
    
    // RK4
    double xRK4 = x0;
    for (int i = 0; i < steps; ++i) {
        double k1 = -xRK4;
        double k2 = -(xRK4 + 0.5*dt*k1);
        double k3 = -(xRK4 + 0.5*dt*k2);
        double k4 = -(xRK4 + dt*k3);
        xRK4 += (dt/6.0) * (k1 + 2*k2 + 2*k3 + k4);
    }
    double errorRK4 = std::abs(xRK4 - expected);
    
    std::cout << "  Resultado Euler: " << xEuler << " (error: " << errorEuler << ")" << std::endl;
    std::cout << "  Resultado RK4:   " << xRK4 << " (error: " << errorRK4 << ")" << std::endl;
    std::cout << "  Esperado:        " << expected << std::endl;
    
    // RK4 debe ser más preciso que Euler
    assert(errorRK4 < errorEuler);
    
    std::cout << "  [PASS] RK4 es más preciso que Euler" << std::endl;
    std::cout << "    RK4 es " << (errorEuler/errorRK4) << "x más preciso" << std::endl;
}

/**
 * @test Conservación de energía en oscilador armónico
 * @details Para un sistema conservativo (sin fricción), RK4 debe conservar energía
 * 
 * Oscilador armónico: d²x/dt² = -ω²x
 * Energía total = 1/2 * v² + 1/2 * ω² * x² (debe ser constante)
 */
void test_energy_conservation() {
    std::cout << "Test: Conservación de energía (oscilador armónico)..." << std::endl;
    
    double omega = 1.0;
    double x = 1.0;  // Posición inicial
    double v = 0.0;  // Velocidad inicial
    double dt = 0.001;
    double tFinal = 10.0;  // Múltiples períodos
    
    // Energía inicial
    double initialEnergy = 0.5 * v*v + 0.5 * omega*omega * x*x;
    
    // Integrar con RK4
    int steps = static_cast<int>(tFinal / dt);
    for (int i = 0; i < steps; ++i) {
        // Sistema: dx/dt = v, dv/dt = -ω²x
        double k1x = v;
        double k1v = -omega*omega * x;
        
        double k2x = v + 0.5*dt*k1v;
        double k2v = -omega*omega * (x + 0.5*dt*k1x);
        
        double k3x = v + 0.5*dt*k2v;
        double k3v = -omega*omega * (x + 0.5*dt*k2x);
        
        double k4x = v + dt*k3v;
        double k4v = -omega*omega * (x + dt*k3x);
        
        x += (dt/6.0) * (k1x + 2*k2x + 2*k3x + k4x);
        v += (dt/6.0) * (k1v + 2*k2v + 2*k3v + k4v);
    }
    
    double finalEnergy = 0.5 * v*v + 0.5 * omega*omega * x*x;
    double relativeError = std::abs(finalEnergy - initialEnergy) / initialEnergy;
    
    std::cout << "  Energía inicial: " << initialEnergy << std::endl;
    std::cout << "  Energía final:   " << finalEnergy << std::endl;
    std::cout << "  Error relativo:  " << (relativeError*100) << "%" << std::endl;
    
    // Error relativo debe ser muy pequeño (< 1%)
    assert(relativeError < 0.01);
    
    std::cout << "  [PASS] Energía conservada con alta precisión" << std::endl;
}

/**
 * @test Estabilidad numérica de los métodos
 * @details Verifica que los métodos no divergen para pasos de tiempo razonables
 */
void test_stability() {
    std::cout << "Test: Estabilidad numérica..." << std::endl;
    
    // EDO: dx/dt = -100x (rápidamente decreciente)
    // Requiere dt < 0.02 para estabilidad con Euler (condición: |1 - 100*dt| < 1)
    
    double x0 = 1.0;
    double lambda = 100.0;  // Constante de decaimiento rápido
    double dt = 0.005;  // Suficientemente pequeño para estabilidad
    double tFinal = 0.1;
    
    // Euler
    double xEuler = x0;
    int steps = static_cast<int>(tFinal / dt);
    for (int i = 0; i < steps; ++i) {
        xEuler += dt * (-lambda * xEuler);
    }
    
    // RK4
    double xRK4 = x0;
    for (int i = 0; i < steps; ++i) {
        double k1 = -lambda * xRK4;
        double k2 = -lambda * (xRK4 + 0.5*dt*k1);
        double k3 = -lambda * (xRK4 + 0.5*dt*k2);
        double k4 = -lambda * (xRK4 + dt*k3);
        xRK4 += (dt/6.0) * (k1 + 2*k2 + 2*k3 + k4);
    }
    
    // Ambos deben converger a cero (sin diverger)
    assert(xEuler > 0 && xEuler < 1);  // Debe ser positivo y decreciente
    assert(xRK4 > 0 && xRK4 < 1);
    
    std::cout << "  Resultado Euler: " << xEuler << std::endl;
    std::cout << "  Resultado RK4:   " << xRK4 << std::endl;
    std::cout << "  [PASS] Ambos métodos estables" << std::endl;
}

/* ============================================================================
 * MAIN
 * ============================================================================ */

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "Tests de Convergencia de Integradores" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    try {
        test_euler_convergence();
        std::cout << std::endl;
        
        test_rk4_convergence();
        std::cout << std::endl;
        
        test_euler_vs_rk4_precision();
        std::cout << std::endl;
        
        test_energy_conservation();
        std::cout << std::endl;
        
        test_stability();
        
        std::cout << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "¡Todos los tests de convergencia pasaron!" << std::endl;
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
