add_library(tp_simulation
    models/PhysicsModel.cpp
    integrators/Integrators.cpp
    runner/SimulationRunner.cpp
)

target_include_directories(tp_simulation
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../shared
        ${CMAKE_CURRENT_SOURCE_DIR}/../domain
)

target_link_libraries(tp_simulation
    PUBLIC
        tp_domain
        tp_shared
)

target_compile_features(tp_simulation PUBLIC cxx_std_20)
