#include <iostream>
#include <filesystem>
#include <vector>
#include <memory>
#include "Logger.hpp"
#include "Scenario.hpp"
#include "VectorField.hpp"
#include "Boat.hpp"
#include "Motor.hpp"
#include "models/PhysicsModel.hpp"
#include "integrators/Integrators.hpp"
#include "runner/SimulationRunner.hpp"

using namespace tp::shared;
using namespace tp::domain;
using namespace tp::simulation;

void waitForKey() {
    std::cout << "\n";
    std::cout << "========================================" << std::endl;
    std::cout << "Presione ENTER para continuar...";
    std::cin.get();
}

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

tp::simulation::SimulationResult lastResult;
bool hasLastResult = false;

void showMenu() {
    clearScreen();
    std::cout << "========================================" << std::endl;
    std::cout << "  TALLER PARAMETRICO - MENU PRINCIPAL" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    std::cout << "1. Simulacion rapida (demo)" << std::endl;
    std::cout << "2. Simulacion personalizada" << std::endl;
    std::cout << "3. Comparar metodos numericos" << std::endl;
    std::cout << "4. Ver informacion del sistema" << std::endl;
    if (hasLastResult) {
        std::cout << "5. Exportar ultimos resultados (CSV)" << std::endl;
        std::cout << "6. Salir" << std::endl;
    } else {
        std::cout << "5. Salir" << std::endl;
    }
    std::cout << std::endl;
    std::cout << "Seleccione una opcion: ";
}

void runQuickSimulation() {
    LOG_INFO_MACRO("========================================");
    LOG_INFO_MACRO("Simulacion Rapida - Demo");
    LOG_INFO_MACRO("========================================");
    
    // Configuracion demo
    Scenario scenario(50, 50);
    VectorField field = VectorField::uniform(2.0, 0.0);
    Boat boat;
    boat.setPosition(Vec2d(10.0, 25.0));
    boat.setMass(1.0);
    boat.setDragCoefficient(0.1);
    Motor motor = Motor::constant(5.0, 0.0);
    
    LOG_INFO_MACRO("Configuracion:");
    LOG_INFO_MACRO("  - Escenario: 50x50");
    LOG_INFO_MACRO("  - Campo: Uniforme (2.0, 0.0)");
    LOG_INFO_MACRO("  - Bote: Masa 1.0, Pos (10, 25)");
    LOG_INFO_MACRO("  - Motor: Empuje 5.0");
    LOG_INFO_MACRO("  - Metodo: Euler");
    
    SimulationRunner runner(boat, field, motor, scenario);
    runner.setTimeStep(0.01);
    runner.setMaxTime(5.0);
    runner.setIntegrator(std::make_unique<EulerIntegrator>());
    
    auto result = runner.run();
    
    LOG_INFO_MACRO("");
    LOG_INFO_MACRO("RESULTADOS:");
    LOG_INFO_MACRO("  Tiempo final:      " + std::to_string(result.finalTime) + " s");
    LOG_INFO_MACRO("  Trabajo total:     " + std::to_string(result.totalWork) + " J");
    LOG_INFO_MACRO("  Velocidad maxima:  " + std::to_string(result.maxSpeed) + " m/s");
    LOG_INFO_MACRO("  Distancia total:   " + std::to_string(result.totalDistance) + " m");
    LOG_INFO_MACRO("  Estados:           " + std::to_string(result.states.size()));
    LOG_INFO_MACRO("  Colision:          " + std::string(result.endedByCollision ? "SI" : "NO"));
    
    waitForKey();
}

void runCustomSimulation() {
    LOG_INFO_MACRO("========================================");
    LOG_INFO_MACRO("Simulacion Personalizada");
    LOG_INFO_MACRO("========================================");
    
    std::cout << "\nConfiguracion del Escenario:" << std::endl;
    std::cout << "  Ancho (default 50): ";
    int width;
    std::cin >> width;
    if (width <= 0) width = 50;
    
    std::cout << "  Alto (default 50): ";
    int height;
    std::cin >> height;
    if (height <= 0) height = 50;
    
    Scenario scenario(width, height);
    
    std::cout << "\nTipo de Campo Vectorial:" << std::endl;
    std::cout << "  1. Uniforme" << std::endl;
    std::cout << "  2. Radial" << std::endl;
    std::cout << "  3. Rotacional" << std::endl;
    std::cout << "  Seleccion: ";
    
    int fieldType;
    std::cin >> fieldType;
    
    VectorField field;
    switch (fieldType) {
        case 1:
            field = VectorField::uniform(2.0, 0.0);
            LOG_INFO_MACRO("Campo: Uniforme");
            break;
        case 2:
            field = VectorField::radial(width/2.0, height/2.0, 0.5);
            LOG_INFO_MACRO("Campo: Radial");
            break;
        case 3:
            field = VectorField::rotational(width/2.0, height/2.0, 1.0);
            LOG_INFO_MACRO("Campo: Rotacional");
            break;
        default:
            field = VectorField::uniform(0.0, 0.0);
            LOG_INFO_MACRO("Campo: Ninguno");
    }
    
    std::cout << "\nPosicion inicial del bote (x y): ";
    double x, y;
    std::cin >> x >> y;
    
    Boat boat;
    boat.setPosition(Vec2d(x, y));
    boat.setMass(1.0);
    boat.setDragCoefficient(0.1);
    
    std::cout << "Empuje del motor: ";
    double thrust;
    std::cin >> thrust;
    
    Motor motor = Motor::constant(thrust, 0.0);
    
    std::cout << "Tiempo maximo de simulacion (s): ";
    double maxTime;
    std::cin >> maxTime;
    
    SimulationRunner runner(boat, field, motor, scenario);
    runner.setTimeStep(0.01);
    runner.setMaxTime(maxTime);
    runner.setIntegrator(std::make_unique<EulerIntegrator>());
    
    LOG_INFO_MACRO("Ejecutando simulacion...");
    auto result = runner.run();
    
    LOG_INFO_MACRO("");
    LOG_INFO_MACRO("RESULTADOS:");
    LOG_INFO_MACRO("  Tiempo final:      " + std::to_string(result.finalTime) + " s");
    LOG_INFO_MACRO("  Trabajo total:     " + std::to_string(result.totalWork) + " J");
    LOG_INFO_MACRO("  Velocidad maxima:  " + std::to_string(result.maxSpeed) + " m/s");
    LOG_INFO_MACRO("  Distancia total:   " + std::to_string(result.totalDistance) + " m");
    LOG_INFO_MACRO("  Estados:           " + std::to_string(result.states.size()));
    LOG_INFO_MACRO("  Colision:          " + std::string(result.endedByCollision ? "SI" : "NO"));
    
    waitForKey();
}

void compareMethods() {
    LOG_INFO_MACRO("========================================");
    LOG_INFO_MACRO("Comparacion de Metodos Numericos");
    LOG_INFO_MACRO("========================================");
    
    // Configuracion estandar
    Scenario scenario(50, 50);
    VectorField field = VectorField::uniform(2.0, 0.0);
    Boat boat;
    boat.setPosition(Vec2d(10.0, 25.0));
    boat.setMass(1.0);
    boat.setDragCoefficient(0.1);
    Motor motor = Motor::constant(5.0, 0.0);
    
    LOG_INFO_MACRO("\nEjecutando simulaciones con dt=0.01s, t_max=5.0s...\n");
    
    // Euler
    {
        SimulationRunner runner(boat, field, motor, scenario);
        runner.setTimeStep(0.01);
        runner.setMaxTime(5.0);
        runner.setIntegrator(std::make_unique<EulerIntegrator>());
        
        auto result = runner.run();
        
        LOG_INFO_MACRO("Euler:");
        LOG_INFO_MACRO("  Estados:          " + std::to_string(result.states.size()));
        LOG_INFO_MACRO("  Velocidad max:    " + std::to_string(result.maxSpeed) + " m/s");
        LOG_INFO_MACRO("  Distancia:        " + std::to_string(result.totalDistance) + " m");
        LOG_INFO_MACRO("  Trabajo:          " + std::to_string(result.totalWork) + " J");
        LOG_INFO_MACRO("");
    }
    
    // Euler Mejorado
    {
        SimulationRunner runner(boat, field, motor, scenario);
        runner.setTimeStep(0.01);
        runner.setMaxTime(5.0);
        runner.setIntegrator(std::make_unique<ImprovedEulerIntegrator>());
        
        auto result = runner.run();
        
        LOG_INFO_MACRO("Euler Mejorado:");
        LOG_INFO_MACRO("  Estados:          " + std::to_string(result.states.size()));
        LOG_INFO_MACRO("  Velocidad max:    " + std::to_string(result.maxSpeed) + " m/s");
        LOG_INFO_MACRO("  Distancia:        " + std::to_string(result.totalDistance) + " m");
        LOG_INFO_MACRO("  Trabajo:          " + std::to_string(result.totalWork) + " J");
        LOG_INFO_MACRO("");
    }
    
    // RK4
    {
        SimulationRunner runner(boat, field, motor, scenario);
        runner.setTimeStep(0.01);
        runner.setMaxTime(5.0);
        runner.setIntegrator(std::make_unique<RK4Integrator>());
        
        auto result = runner.run();
        
        LOG_INFO_MACRO("RK4:");
        LOG_INFO_MACRO("  Estados:          " + std::to_string(result.states.size()));
        LOG_INFO_MACRO("  Velocidad max:    " + std::to_string(result.maxSpeed) + " m/s");
        LOG_INFO_MACRO("  Distancia:        " + std::to_string(result.totalDistance) + " m");
        LOG_INFO_MACRO("  Trabajo:          " + std::to_string(result.totalWork) + " J");
        LOG_INFO_MACRO("");
    }
    
    waitForKey();
}

void showSystemInfo() {
    LOG_INFO_MACRO("========================================");
    LOG_INFO_MACRO("Informacion del Sistema");
    LOG_INFO_MACRO("========================================");
    LOG_INFO_MACRO("Taller Parametrico de Navegacion y Campos 2D");
    LOG_INFO_MACRO("Version: 0.1.0");
    LOG_INFO_MACRO("");
    LOG_INFO_MACRO("Caracteristicas implementadas:");
    LOG_INFO_MACRO("  - Campos vectoriales: Uniforme, Radial, Rotacional");
    LOG_INFO_MACRO("  - Metodos numericos: Euler, Euler Mejorado, RK4");
    LOG_INFO_MACRO("  - Modelo fisico: Empuje + Arrastre + Campo");
    LOG_INFO_MACRO("  - Calculo de energia y trabajo");
    LOG_INFO_MACRO("  - Deteccion de colisiones");
    LOG_INFO_MACRO("  - Sistema de logging completo");
    LOG_INFO_MACRO("");
    LOG_INFO_MACRO("Archivo de log: logs/console_test.log");
    
    waitForKey();
}

int main() {
    // Inicializar logging
    std::filesystem::create_directories("logs");
    Logger::instance().setOutputFile("logs/console_test.log");
    Logger::instance().setLogLevel(LogLevel::LOG_INFO);
    Logger::instance().enableConsoleOutput(true);
    
    LOG_INFO_MACRO("========================================");
    LOG_INFO_MACRO("Taller Parametrico - Iniciando");
    LOG_INFO_MACRO("========================================");
    
    bool running = true;
    
    while (running) {
        showMenu();
        
        int option;
        std::cin >> option;
        std::cin.ignore(); // Limpiar buffer
        
        try {
            switch (option) {
                case 1:
                    runQuickSimulation();
                    break;
                case 2:
                    runCustomSimulation();
                    break;
                case 3:
                    compareMethods();
                    break;
                case 4:
                    showSystemInfo();
                    break;
                case 5:
                    running = false;
                    LOG_INFO_MACRO("Cerrando aplicacion...");
                    break;
                default:
                    LOG_WARNING("Opcion invalida");
                    waitForKey();
            }
        } catch (const std::exception& e) {
            LOG_FATAL(std::string("Error: ") + e.what());
            waitForKey();
        }
    }
    
    LOG_INFO_MACRO("========================================");
    LOG_INFO_MACRO("Hasta luego!");
    LOG_INFO_MACRO("========================================");
    
    waitForKey();
    return 0;
}
