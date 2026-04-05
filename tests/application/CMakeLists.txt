# Tests de Application
# Tests para la capa de aplicación (coordinadores, servicios, repositorios)
# Cada test es un ejecutable independiente para evitar conflictos de main()

# Test de ExperimentService
add_executable(test_experiment_service
    test_experiment_service.cpp
)

target_link_libraries(test_experiment_service
    PRIVATE
        tp_application
        tp_domain
        tp_simulation
)

add_test(NAME ExperimentServiceTests COMMAND test_experiment_service)

# Test de SimulationCoordinator
add_executable(test_simulation_coordinator
    test_simulation_coordinator.cpp
)

target_link_libraries(test_simulation_coordinator
    PRIVATE
        tp_application
        tp_domain
        tp_simulation
)

add_test(NAME SimulationCoordinatorTests COMMAND test_simulation_coordinator)
