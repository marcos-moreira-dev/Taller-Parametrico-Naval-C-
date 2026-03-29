add_library(tp_rendering INTERFACE)

target_include_directories(tp_rendering
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(tp_rendering
    INTERFACE
        tp_domain
        tp_shared
)

target_compile_features(tp_rendering INTERFACE cxx_std_20)
