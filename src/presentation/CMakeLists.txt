add_library(tp_presentation
    main_window/MainWindow.cpp
    widgets/SimulationCanvas.cpp
    widgets/SimpleGraph.cpp
    widgets/FieldArrowSampler.cpp
    widgets/MathGraph.cpp
    widgets/TheoryGraphFactory.cpp
    widgets/MathHtmlPanel.cpp
    widgets/LatexBitmapPanel.cpp
    widgets/ToastNotification.cpp
    widgets/WaterParticleSystem.cpp
    widgets/EquationRenderer.cpp
    widgets/VideoExporter.cpp
    widgets/WaterAnimationSystem.cpp
    widgets/Water3DCanvas.cpp
    panels/TheoryPanel.cpp
    panels/EditorPanel.cpp
    panels/FieldEditorPanel.cpp
)

find_package(OpenGL REQUIRED)

target_include_directories(tp_presentation
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../shared
        ${CMAKE_CURRENT_SOURCE_DIR}/../application
        ${CMAKE_CURRENT_SOURCE_DIR}/../domain
        ${wxWidgets_INCLUDE_DIRS}
        ${wxWidgets_LIBRARY_DIRS}/mswu
        $<TARGET_PROPERTY:tp_shared,INTERFACE_INCLUDE_DIRECTORIES>
)

target_compile_definitions(tp_presentation PUBLIC _ssize_t=long TP_SOURCE_DIR="${CMAKE_SOURCE_DIR}")

target_link_libraries(tp_presentation
    PUBLIC
        tp_application
        tp_education
        tp_shared
        ${wxWidgets_LIBRARIES}
        opengl32
        glu32
        gdi32
)

target_compile_features(tp_presentation PUBLIC cxx_std_20)

if(WIN32)
    target_link_options(tp_presentation PUBLIC -mwindows)
endif()
