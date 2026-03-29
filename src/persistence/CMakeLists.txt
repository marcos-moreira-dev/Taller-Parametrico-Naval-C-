add_library(tp_persistence
    ScenarioLoader.cpp
    ScenarioSaver.cpp
)

target_include_directories(tp_persistence
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../shared
        ${CMAKE_CURRENT_SOURCE_DIR}/../domain
)

target_link_libraries(tp_persistence
    PUBLIC
        tp_domain
        tp_shared
)

target_compile_features(tp_persistence PUBLIC cxx_std_20)
