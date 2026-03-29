#include "ExperimentService.hpp"
#include "ScenarioLoader.hpp"
#include "ScenarioSaver.hpp"
#include "integrators/Integrators.hpp"
#include "Logger.hpp"

namespace tp::application {

using namespace tp::shared;

ExperimentService::ExperimentService()
    : runner_(nullptr) {
    config_.scenario = Scenario(50, 50);
    config_.field = VectorField::uniform(0.0, 0.0);
    config_.fieldView = {FieldPresetType::Uniform, 0.0, 25.0, 25.0};
    config_.boat = Boat();
    config_.motor = Motor::constant(0.0, 0.0);
    config_.timeStep = 0.01;
    config_.maxTime = 10.0;
    config_.method = IntegrationMethod::Euler;
    initPresets();
    LOG_INFO_MACRO("ExperimentService inicializado");
}

void ExperimentService::createNewExperiment() {
    LOG_INFO_MACRO("Creando nuevo experimento...");
    
    config_.scenario = Scenario(50, 50);
    config_.field = VectorField::uniform(0.0, 0.0);
    config_.fieldView = {FieldPresetType::Uniform, 0.0, 25.0, 25.0};
    config_.boat = Boat();
    config_.motor = Motor::constant(0.0, 0.0);
    
    LOG_DEBUG_MACRO("Configuracion inicial del experimento:");
    LOG_DEBUG_MACRO("  - Dimensiones del escenario: 50x50");
    LOG_DEBUG_MACRO("  - Campo: uniforme (0, 0)");
    LOG_DEBUG_MACRO("  - Bote: valores por defecto");
    LOG_DEBUG_MACRO("  - Motor: constante 0.0");
    LOG_INFO_MACRO("Nuevo experimento creado");
}

void ExperimentService::loadExperiment(const std::string& filepath) {
    LOG_INFO_MACRO("Cargando experimento desde: " + filepath);
    
    auto scenario = persistence::ScenarioLoader::loadFromFile(filepath);
    if (scenario) {
        config_.scenario = *scenario;
        LOG_INFO_MACRO("Experimento cargado exitosamente");
    } else {
        LOG_ERROR_MACRO("No se pudo cargar el experimento desde: " + filepath);
    }
}

void ExperimentService::saveExperiment(const std::string& filepath) {
    LOG_INFO_MACRO("Guardando experimento en: " + filepath);
    
    if (persistence::ScenarioSaver::saveToFile(config_.scenario, filepath)) {
        LOG_INFO_MACRO("Experimento guardado exitosamente");
    } else {
        LOG_ERROR_MACRO("No se pudo guardar el experimento en: " + filepath);
    }
}

void ExperimentService::setScenario(const Scenario& scenario) {
    LOG_DEBUG_MACRO("Configurando escenario...");
    config_.scenario = scenario;
    LOG_DEBUG_MACRO("Escenario configurado");
    notifyConfigChange();
}

void ExperimentService::setField(const VectorField& field) {
    LOG_DEBUG_MACRO("Configurando campo vectorial...");
    config_.field = field;
    LOG_DEBUG_MACRO("Campo vectorial configurado");
    notifyConfigChange();
}

void ExperimentService::setBoat(const Boat& boat) {
    LOG_DEBUG_MACRO("Configurando bote...");
    config_.boat = boat;
    LOG_DEBUG_MACRO("Bote configurado");
    notifyConfigChange();
}

void ExperimentService::setMotor(const Motor& motor) {
    LOG_DEBUG_MACRO("Configurando motor...");
    config_.motor = motor;
    LOG_DEBUG_MACRO("Motor configurado");
    notifyConfigChange();
}

simulation::SimulationResult ExperimentService::runSimulation() {
    LOG_INFO_MACRO("Preparando simulacion...");
    LOG_INFO_MACRO("Parametros de simulacion:");
    LOG_INFO_MACRO("  - Paso temporal: " + std::to_string(config_.timeStep));
    LOG_INFO_MACRO("  - Tiempo maximo: " + std::to_string(config_.maxTime));
    LOG_INFO_MACRO("  - Metodo: " + std::string(config_.method == IntegrationMethod::Euler ? "Euler" : "RK4"));
    
    runner_ = std::make_unique<simulation::SimulationRunner>(
        config_.boat, config_.field, config_.motor, config_.scenario
    );
    
    runner_->setTimeStep(config_.timeStep);
    runner_->setMaxTime(config_.maxTime);
    
    std::unique_ptr<simulation::Integrator> integrator;
    switch (config_.method) {
        case IntegrationMethod::Euler:
            LOG_DEBUG_MACRO("Usando integrador Euler");
            integrator = std::make_unique<simulation::EulerIntegrator>();
            break;
        case IntegrationMethod::RK4:
            LOG_DEBUG_MACRO("Usando integrador RK4");
            integrator = std::make_unique<simulation::RK4Integrator>();
            break;
        default:
            LOG_WARNING("Metodo desconocido, usando Euler por defecto");
            integrator = std::make_unique<simulation::EulerIntegrator>();
    }
    
    runner_->setIntegrator(std::move(integrator));
    
    LOG_INFO_MACRO("Ejecutando simulacion...");
    auto result = runner_->run();
    LOG_INFO_MACRO("Simulacion finalizada");
    
    return result;
}

void ExperimentService::initPresets() {
    presets_.push_back({
        "1. Río con corriente uniforme",
        [](ExperimentConfig& config) {
            config.field = VectorField::uniform(2.0, 0.0);
            config.fieldView = {FieldPresetType::Uniform, 2.0, 25.0, 25.0};
            config.boat.setPosition(Vec2d(5.0, 25.0));
            config.boat.setOrientation(0.0);
            config.scenario = Scenario(50, 50);
            for (int y = 0; y < 50; ++y) {
                for (int x = 0; x < 50; ++x) {
                    config.scenario.setCell(x, y, CellType::Water);
                }
            }
        }
    });
    
    presets_.push_back({
        "2. Remolino central",
        [](ExperimentConfig& config) {
            config.field = VectorField::rotational(25.0, 25.0, 1.0);
            config.fieldView = {FieldPresetType::Rotational, 1.0, 25.0, 25.0};
            config.boat.setPosition(Vec2d(15.0, 25.0));
            config.boat.setOrientation(0.0);
            config.scenario = Scenario(50, 50);
            for (int y = 0; y < 50; ++y) {
                for (int x = 0; x < 50; ++x) {
                    config.scenario.setCell(x, y, CellType::Water);
                }
            }
        }
    });
    
    presets_.push_back({
        "3. Canal con obstáculo",
        [](ExperimentConfig& config) {
            config.field = VectorField::uniform(1.5, 0.0);
            config.fieldView = {FieldPresetType::Uniform, 1.5, 25.0, 25.0};
            config.boat.setPosition(Vec2d(5.0, 25.0));
            config.boat.setOrientation(0.0);
            config.scenario = Scenario(50, 50);
            for (int y = 0; y < 50; ++y) {
                for (int x = 0; x < 50; ++x) {
                    if (y < 10 || y >= 40) {
                        config.scenario.setCell(x, y, CellType::Land);
                    } else {
                        config.scenario.setCell(x, y, CellType::Water);
                    }
                }
            }
            for (int y = 20; y < 30; ++y) {
                for (int x = 20; x < 25; ++x) {
                    config.scenario.setCell(x, y, CellType::Obstacle);
                }
            }
        }
    });
    
    presets_.push_back({
        "4. Zona turbulenta",
        [](ExperimentConfig& config) {
            config.field = VectorField::uniform(3.0, 0.0);
            config.fieldView = {FieldPresetType::Uniform, 3.0, 25.0, 25.0};
            config.boat.setPosition(Vec2d(5.0, 25.0));
            config.boat.setOrientation(0.0);
            config.scenario = Scenario(50, 50);
            for (int y = 0; y < 50; ++y) {
                for (int x = 0; x < 50; ++x) {
                    config.scenario.setCell(x, y, CellType::Water);
                }
            }
            for (int i = 0; i < 15; ++i) {
                int x = 10 + (i * 3) % 35;
                int y = 15 + (i * 5) % 20;
                config.scenario.setCell(x, y, CellType::Obstacle);
                config.scenario.setCell(x+1, y, CellType::Obstacle);
                config.scenario.setCell(x, y+1, CellType::Obstacle);
            }
        }
    });
    
    presets_.push_back({
        "5. Laberinto de canales",
        [](ExperimentConfig& config) {
            config.field = VectorField::uniform(1.0, 0.0);
            config.fieldView = {FieldPresetType::Uniform, 1.0, 25.0, 25.0};
            config.boat.setPosition(Vec2d(5.0, 5.0));
            config.boat.setOrientation(0.0);
            config.scenario = Scenario(50, 50);
            for (int y = 0; y < 50; ++y) {
                for (int x = 0; x < 50; ++x) {
                    config.scenario.setCell(x, y, CellType::Land);
                }
            }
            for (int x = 0; x < 50; ++x) {
                config.scenario.setCell(x, 5, CellType::Water);
                config.scenario.setCell(x, 15, CellType::Water);
                config.scenario.setCell(x, 25, CellType::Water);
                config.scenario.setCell(x, 35, CellType::Water);
                config.scenario.setCell(x, 45, CellType::Water);
            }
            for (int y = 5; y <= 15; ++y) config.scenario.setCell(10, y, CellType::Water);
            for (int y = 15; y <= 25; ++y) config.scenario.setCell(25, y, CellType::Water);
            for (int y = 25; y <= 35; ++y) config.scenario.setCell(40, y, CellType::Water);
            for (int y = 35; y <= 45; ++y) config.scenario.setCell(15, y, CellType::Water);
        }
    });
    
    presets_.push_back({
        "6. Zona de rápidos",
        [](ExperimentConfig& config) {
            config.field = VectorField::uniform(4.0, 0.0);
            config.fieldView = {FieldPresetType::Uniform, 4.0, 25.0, 25.0};
            config.boat.setPosition(Vec2d(5.0, 25.0));
            config.boat.setOrientation(0.0);
            config.scenario = Scenario(50, 50);
            for (int y = 0; y < 50; ++y) {
                for (int x = 0; x < 50; ++x) {
                    config.scenario.setCell(x, y, CellType::Water);
                }
            }
            for (int x = 0; x < 50; ++x) {
                if (x % 8 < 4) {
                    config.scenario.setCell(x, 15, CellType::Land);
                    config.scenario.setCell(x, 35, CellType::Land);
                }
            }
        }
    });
    
    presets_.push_back({
        "7. Efecto Venturi",
        [](ExperimentConfig& config) {
            config.field = VectorField::uniform(1.0, 0.0);
            config.fieldView = {FieldPresetType::Uniform, 1.0, 30.0, 15.0};
            config.boat.setPosition(Vec2d(5.0, 25.0));
            config.boat.setOrientation(0.0);
            config.scenario = Scenario(60, 30);
            for (int y = 0; y < 30; ++y) {
                for (int x = 0; x < 60; ++x) {
                    config.scenario.setCell(x, y, CellType::Water);
                }
            }
            for (int x = 15; x < 45; ++x) {
                int margin = (x < 25 || x >= 35) ? 0 : (x < 30 ? 25 - x : x - 35);
                if (x < 25 || x >= 35) {
                    if (x < 20 || x >= 40) {
                        config.scenario.setCell(x, 5, CellType::Land);
                        config.scenario.setCell(x, 24, CellType::Land);
                    } else {
                        int offset = (x < 30) ? 25 - x : x - 35;
                        if (x < 30) {
                            config.scenario.setCell(x, 5 + offset, CellType::Land);
                            config.scenario.setCell(x, 24 - offset, CellType::Land);
                        }
                    }
                }
            }
        }
    });
    
    presets_.push_back({
        "8. Puerto con rompeolas",
        [](ExperimentConfig& config) {
            config.field = VectorField::uniform(0.5, 0.0);
            config.fieldView = {FieldPresetType::Uniform, 0.5, 25.0, 25.0};
            config.boat.setPosition(Vec2d(5.0, 25.0));
            config.boat.setOrientation(0.0);
            config.scenario = Scenario(50, 50);
            for (int y = 0; y < 50; ++y) {
                for (int x = 0; x < 50; ++x) {
                    config.scenario.setCell(x, y, CellType::Water);
                }
            }
            for (int x = 0; x < 50; ++x) {
                config.scenario.setCell(x, 0, CellType::Land);
                config.scenario.setCell(x, 1, CellType::Land);
            }
            for (int y = 0; y < 50; ++y) {
                config.scenario.setCell(0, y, CellType::Land);
                config.scenario.setCell(1, y, CellType::Land);
            }
            for (int x = 35; x < 40; ++x) {
                for (int y = 5; y < 45; ++y) {
                    config.scenario.setCell(x, y, CellType::Obstacle);
                }
            }
            for (int x = 40; x < 42; ++x) {
                for (int y = 15; y < 35; ++y) {
                    config.scenario.setCell(x, y, CellType::Obstacle);
                }
            }
        }
    });
    
    presets_.push_back({
        "9. Corrientes en capas",
        [](ExperimentConfig& config) {
            config.field = VectorField::uniform(2.0, 0.0);
            config.fieldView = {FieldPresetType::Uniform, 2.0, 25.0, 25.0};
            config.boat.setPosition(Vec2d(5.0, 25.0));
            config.boat.setOrientation(0.0);
            config.scenario = Scenario(50, 50);
            for (int y = 0; y < 50; ++y) {
                for (int x = 0; x < 50; ++x) {
                    config.scenario.setCell(x, y, CellType::Water);
                }
            }
            for (int x = 0; x < 50; ++x) {
                config.scenario.setCell(x, 25, CellType::Land);
                config.scenario.setCell(x, 26, CellType::Land);
            }
        }
    });
    
    presets_.push_back({
        "10. Corriente oscilatoria",
        [](ExperimentConfig& config) {
            config.field = VectorField::uniform(2.0, 0.0);
            config.fieldView = {FieldPresetType::Uniform, 2.0, 25.0, 25.0};
            config.boat.setPosition(Vec2d(25.0, 25.0));
            config.boat.setOrientation(0.0);
            config.scenario = Scenario(50, 50);
            for (int y = 0; y < 50; ++y) {
                for (int x = 0; x < 50; ++x) {
                    config.scenario.setCell(x, y, CellType::Water);
                }
            }
            for (int y = 0; y < 50; ++y) {
                config.scenario.setCell(0, y, CellType::Land);
                config.scenario.setCell(1, y, CellType::Land);
                config.scenario.setCell(48, y, CellType::Land);
                config.scenario.setCell(49, y, CellType::Land);
            }
        }
    });

    presets_.push_back({
        "11. Espiral de caracol",
        [](ExperimentConfig& config) {
            const double cx = 25.0;
            const double cy = 25.0;
            config.field = VectorField([cx, cy](double x, double y) {
                const double dx = x - cx;
                const double dy = y - cy;
                return Vec2d(
                    -0.08 * dx - 0.25 * dy,
                    -0.08 * dy + 0.25 * dx
                );
            });
            config.fieldView = {FieldPresetType::Custom, 0.0, cx, cy};
            config.boat.setPosition(Vec2d(40.0, 16.0));
            config.boat.setOrientation(3.14159);
            config.scenario = Scenario(50, 50);
            for (int y = 0; y < 50; ++y) {
                for (int x = 0; x < 50; ++x) {
                    config.scenario.setCell(x, y, CellType::Water);
                }
            }
            for (int y = 0; y < 50; ++y) {
                config.scenario.setCell(0, y, CellType::Land);
                config.scenario.setCell(1, y, CellType::Land);
                config.scenario.setCell(48, y, CellType::Land);
                config.scenario.setCell(49, y, CellType::Land);
            }
        }
    });
}

std::string ExperimentService::getScenarioPresetName(size_t index) const {
    if (index < presets_.size()) {
        return presets_[index].name;
    }
    return "";
}

void ExperimentService::applyScenarioPreset(size_t index) {
    if (index < presets_.size()) {
        presets_[index].configure(config_);
        LOG_INFO_MACRO("Escenario predefinido aplicado: " + presets_[index].name);
    }
}

}
