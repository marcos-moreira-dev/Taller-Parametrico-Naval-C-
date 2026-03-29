add_library(tp_education
    TheoryCatalog.cpp
    TheoryRepository.cpp
    TheoryMarkdownRenderer.cpp
    TheoryHtmlTemplate.cpp
)

target_include_directories(tp_education
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(tp_education
    PUBLIC
        tp_shared
)

target_compile_definitions(tp_education
    PUBLIC
        TP_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)

target_compile_features(tp_education PUBLIC cxx_std_20)
