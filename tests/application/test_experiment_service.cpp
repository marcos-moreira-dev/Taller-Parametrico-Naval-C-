#include <iostream>
#include <cassert>
#include <filesystem>
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

void test_save_load_roundtrip() {
    std::cout << "Test: Save/load roundtrip..." << std::endl;

    ExperimentService service;
    service.createNewExperiment();

    auto& config = service.getConfig();
    config.scenario.setName("Mi escenario con espacios");
    config.scenario.setCell(1, 1, tp::shared::CellType::Land);
    config.scenario.setCell(2, 3, tp::shared::CellType::Obstacle);
    config.boat.setPosition({7.0, 8.0});
    config.boat.setOrientation(1.25);
    config.fieldView.type = FieldPresetType::Rotational;
    config.fieldView.intensity = 2.5;
    config.fieldView.centerX = 12.0;
    config.fieldView.centerY = 16.0;
    config.timeStep = 0.02;
    config.maxTime = 12.0;
    config.method = tp::shared::IntegrationMethod::RK4;
    service.rebuildFieldFromView();

    const std::filesystem::path tempFile = std::filesystem::temp_directory_path() / "tp_roundtrip_test.scenario";
    service.saveExperiment(tempFile.string());

    ExperimentService loaded;
    loaded.loadExperiment(tempFile.string());
    const auto& loadedConfig = loaded.getConfig();

    assert(loadedConfig.scenario.getName() == "Mi escenario con espacios");
    assert(loadedConfig.scenario.getCell(1, 1) == tp::shared::CellType::Land);
    assert(loadedConfig.scenario.getCell(2, 3) == tp::shared::CellType::Obstacle);
    assert(loadedConfig.boat.getPosition().x == 7.0);
    assert(loadedConfig.boat.getPosition().y == 8.0);
    assert(loadedConfig.fieldView.type == FieldPresetType::Rotational);
    assert(loadedConfig.fieldView.intensity == 2.5);
    assert(loadedConfig.fieldView.centerX == 12.0);
    assert(loadedConfig.fieldView.centerY == 16.0);
    assert(loadedConfig.timeStep == 0.02);
    assert(loadedConfig.maxTime == 12.0);
    assert(loadedConfig.method == tp::shared::IntegrationMethod::RK4);

    std::filesystem::remove(tempFile);

    std::cout << "  [PASS] Roundtrip de persistencia correcto" << std::endl;
}

void test_custom_field_rebuild() {
    std::cout << "Test: Custom field rebuild..." << std::endl;

    ExperimentService service;
    service.createNewExperiment();

    auto& config = service.getConfig();
    config.fieldView.type = FieldPresetType::Custom;
    config.fieldView.customFx = "-0.08*(x-25)-0.25*(y-25)";
    config.fieldView.customFy = "-0.08*(y-25)+0.25*(x-25)";

    std::string error;
    const bool ok = service.rebuildFieldFromView(&error);
    assert(ok);
    assert(error.empty());

    const auto value = config.field.getValue(8.0, 18.0);
    assert(std::abs(value.x) > 0.1);
    assert(std::abs(value.y) > 0.1);

    std::cout << "  [PASS] Campo personalizado evaluable: (" << value.x << ", " << value.y << ")" << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "Tests de Application" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        test_experiment_service_creation();
        test_create_new_experiment();
        test_save_load_roundtrip();
        test_custom_field_rebuild();
        
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
