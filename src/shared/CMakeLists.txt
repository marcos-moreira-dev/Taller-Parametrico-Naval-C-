add_library(tp_shared
    Logger.cpp
)

target_include_directories(tp_shared 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(tp_shared PUBLIC cxx_std_20)
