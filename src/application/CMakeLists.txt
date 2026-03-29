add_library(tp_application
    ExperimentService.cpp
    SimulationCoordinator.cpp
    UserPreferences.cpp
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
