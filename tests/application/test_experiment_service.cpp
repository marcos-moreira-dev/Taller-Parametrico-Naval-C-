#include <iostream>
#include <cassert>
#include "ExperimentService.hpp"

using namespace tp::application;

void test_experiment_service_creation() {
    std::cout << "Test: ExperimentService creation..." << std::endl;
    
    ExperimentService service;
    
    std::cout << "  [PASS] ExperimentService creado correctamente" << std::endl;
}

void test_create_new_experiment() {
    std::cout << "Test: Create new experiment..." << std::endl;
    
    ExperimentService service;
    service.createNewExperiment();
    
    auto& config = service.getConfig();
    assert(config.scenario.getWidth() == 50);
    assert(config.scenario.getHeight() == 50);
    
    std::cout << "  [PASS] Nuevo experimento creado correctamente" << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "Tests de Application" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        test_experiment_service_creation();
        test_create_new_experiment();
        
        std::cout << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "Todos los tests pasaron!" << std::endl;
        std::cout << "========================================" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}
