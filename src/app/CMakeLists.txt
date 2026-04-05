# Consola version (only if BUILD_CONSOLE not set to OFF)
if(NOT DEFINED BUILD_CONSOLE OR BUILD_CONSOLE)
    add_executable(TallerParametrico_Console
        console_main.cpp
    )

    target_include_directories(TallerParametrico_Console
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/../shared
            ${CMAKE_CURRENT_SOURCE_DIR}/../domain
            ${CMAKE_CURRENT_SOURCE_DIR}/../simulation
    )

    target_link_libraries(TallerParametrico_Console
        PRIVATE
            tp_simulation
            tp_domain
            tp_shared
    )

    set_target_properties(TallerParametrico_Console PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endif()

# GUI version (only if BUILD_GUI is ON)
if(BUILD_GUI)
    # Compilar archivo de recursos de Windows
    if(WIN32)
        set(RESOURCE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/TallerParametrico.rc)
    endif()
    
    add_executable(TallerParametrico
        main.cpp
        ${RESOURCE_FILE}
    )
    
    target_include_directories(TallerParametrico
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/../shared
            ${CMAKE_CURRENT_SOURCE_DIR}/../presentation
            ${wxWidgets_INCLUDE_DIRS}
            ${wxWidgets_LIBRARY_DIRS}/mswu
    )
    
    target_link_libraries(TallerParametrico
        PRIVATE
            tp_presentation
            tp_shared
            ${wxWidgets_LIBRARIES}
    )
    
    target_compile_definitions(TallerParametrico PRIVATE _ssize_t=long)
    
    set_target_properties(TallerParametrico PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )

    if(EXISTS "${wxWidgets_LIBRARY_DIRS}/WebView2Loader.dll")
        add_custom_command(TARGET TallerParametrico POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${wxWidgets_LIBRARY_DIRS}/WebView2Loader.dll"
                    "$<TARGET_FILE_DIR:TallerParametrico>/WebView2Loader.dll"
        )

        install(FILES "${wxWidgets_LIBRARY_DIRS}/WebView2Loader.dll"
            DESTINATION bin
        )
    endif()
endif()
