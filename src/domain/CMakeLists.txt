add_library(tp_domain
    Scenario.cpp
    VectorField.cpp
    Boat.cpp
    Motor.cpp
)

target_include_directories(tp_domain
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../shared
)

target_link_libraries(tp_domain
    PUBLIC
        tp_shared
)

target_compile_features(tp_domain PUBLIC cxx_std_20)
