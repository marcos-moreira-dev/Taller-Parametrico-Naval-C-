add_library(tp_export
    CSVExporter.cpp
    JSONExporter.cpp
)

target_include_directories(tp_export
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../simulation
)

target_link_libraries(tp_export
    PUBLIC
        tp_simulation
)

target_compile_features(tp_export PUBLIC cxx_std_20)
