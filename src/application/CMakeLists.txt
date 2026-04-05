# @file CMakeLists.txt
# @brief Configuración de construcción del módulo 'application'
# @details Capa de aplicación que coordina entre presentación (UI) y dominio.
#          Implementa patrones como MVC, Observer, y Repository.
#
# MÓDULOS INCLUIDOS:
#   - ExperimentService: Orquestación principal de experimentos
#   - SimulationCoordinator: Coordinación entre runner y UI (patrón Mediator)
#   - UserPreferences: Persistencia de preferencias de usuario
#   - PresetFileRepository: Acceso a escenarios predefinidos desde archivos
#   - ScenarioDocument: Patrón Document para edición de escenarios
#
# NOTA SOBRE ARQUITECTURA:
#   SimulationCoordinator es el único coordinador de simulación. Anteriormente
#   se consideró crear SimulationController, pero eso habría creado redundancia.
#   El patrón Coordinator es suficiente para gestionar el ciclo de vida.

add_library(tp_application
    ExperimentService.cpp          # Servicio principal de experimentos
    SimulationCoordinator.cpp      # Coordina runner con timers (patrón Mediator)
                                   # Gestiona ciclo de vida: start, pause, stop, step
    PresetFileRepository.cpp       # Repository pattern para presets
    ScenarioDocument.cpp           # Patrón Document para undo/redo
    UserPreferences.cpp            # Preferencias de usuario
)

target_include_directories(tp_application
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../shared
        ${CMAKE_CURRENT_SOURCE_DIR}/../domain
        ${CMAKE_CURRENT_SOURCE_DIR}/../simulation
        ${CMAKE_CURRENT_SOURCE_DIR}/../persistence
)

target_link_libraries(tp_application
    PUBLIC
        tp_domain
        tp_simulation
        tp_persistence
        tp_shared
)

target_compile_features(tp_application PUBLIC cxx_std_20)

target_compile_definitions(tp_application PUBLIC TP_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
