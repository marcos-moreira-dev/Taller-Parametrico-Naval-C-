cmake_minimum_required(VERSION 3.20)

project(TallerParametrico 
    VERSION 0.1.0
    DESCRIPTION "Taller Parametrico de Navegacion y Campos 2D"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MINGW)
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# En Windows, usar librerías estáticas de runtime para evitar dependencias de DLLs
if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-libgcc -static-libstdc++")
endif()

option(BUILD_TESTS "Build tests" ON)
option(BUILD_GUI "Build GUI application" ON)

include(FetchContent)

# Add modules
add_subdirectory(src/shared)
add_subdirectory(src/domain)
add_subdirectory(src/simulation)
add_subdirectory(src/persistence)
add_subdirectory(src/application)
add_subdirectory(src/education)

if(BUILD_GUI)
    # Buscar wxWidgets en múltiples ubicaciones
    # NOTA: wx_installed está en la raíz del proyecto, no en build/
    set(WX_INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/wx_installed")
    set(WX_BUILD_TREE "${CMAKE_CURRENT_SOURCE_DIR}/wxwidgets")
    set(WX_INSTALL_DIR2 "${CMAKE_CURRENT_SOURCE_DIR}/build/wx_installed")
    set(WX_INSTALL_DIR3 "${CMAKE_CURRENT_BINARY_DIR}/wx_installed")
    set(WX_DOWNLOAD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../wxWidgets-3.3.2")
    
    message(STATUS "Buscando wxWidgets en: ${WX_INSTALL_DIR}")
    
    # Primero intentar usar un build tree con wxWebView habilitado.
    if(EXISTS "${WX_BUILD_TREE}/lib/gcc_x64_lib/wxmsw33u_webview.a" AND EXISTS "${WX_DOWNLOAD_DIR}/include/wx/webview.h")
        message(STATUS "wxWidgets con webview encontrado en: ${WX_BUILD_TREE}")
        set(wxWidgets_FOUND TRUE)
        set(wxWidgets_ROOT_DIR ${WX_BUILD_TREE})
        set(wxWidgets_INCLUDE_DIRS ${WX_DOWNLOAD_DIR}/include)
        set(wxWidgets_LIBRARY_DIRS ${WX_BUILD_TREE}/lib/gcc_x64_lib)
        set(wxWidgets_LIBRARIES
            ${WX_BUILD_TREE}/lib/gcc_x64_lib/wxmsw33u_webview.a
            ${WX_BUILD_TREE}/lib/gcc_x64_lib/wxmsw33u_aui.a
            ${WX_BUILD_TREE}/lib/gcc_x64_lib/wxmsw33u_html.a
            ${WX_BUILD_TREE}/lib/gcc_x64_lib/wxmsw33u_adv.a
            ${WX_BUILD_TREE}/lib/gcc_x64_lib/wxmsw33u_core.a
            ${WX_BUILD_TREE}/lib/gcc_x64_lib/wxbase33u_net.a
            ${WX_BUILD_TREE}/lib/gcc_x64_lib/wxbase33u.a
            ${WX_BUILD_TREE}/lib/gcc_x64_lib/wxtiff.a
            ${WX_BUILD_TREE}/lib/gcc_x64_lib/wxjpeg.a
            ${WX_BUILD_TREE}/lib/gcc_x64_lib/wxpng.a
            ${WX_BUILD_TREE}/lib/gcc_x64_lib/wxzlib.a
            ${WX_BUILD_TREE}/lib/gcc_x64_lib/wxregexu.a
            ${WX_BUILD_TREE}/lib/gcc_x64_lib/wxexpat.a
            comctl32 uuid uxtheme version shlwapi gdi32 gdiplus msimg32 oleacc rpcrt4 winmm oleaut32 imm32
        )
        message(STATUS "Librerías wxWidgets configuradas con webview")
        add_subdirectory(src/presentation)
    # Segundo intento: wxWidgets instalado/compilado tradicional
    elseif(EXISTS "${WX_INSTALL_DIR}/include/wx/wx.h")
        message(STATUS "wxWidgets encontrado en: ${WX_INSTALL_DIR}")
        set(wxWidgets_FOUND TRUE)
        set(wxWidgets_ROOT_DIR ${WX_INSTALL_DIR})
        set(wxWidgets_INCLUDE_DIRS ${WX_INSTALL_DIR}/include)
        set(wxWidgets_LIBRARY_DIRS ${WX_INSTALL_DIR}/lib/gcc_x64_lib)
        # Especificar librerías wxWidgets explícitamente (orden: dependientes primero)
        set(wxWidgets_LIBRARIES 
            ${WX_INSTALL_DIR}/lib/gcc_x64_lib/wxmsw33u_aui.a
            ${WX_INSTALL_DIR}/lib/gcc_x64_lib/wxmsw33u_html.a
            ${WX_INSTALL_DIR}/lib/gcc_x64_lib/wxmsw33u_adv.a
            ${WX_INSTALL_DIR}/lib/gcc_x64_lib/wxmsw33u_core.a
            ${WX_INSTALL_DIR}/lib/gcc_x64_lib/wxbase33u_net.a
            ${WX_INSTALL_DIR}/lib/gcc_x64_lib/wxbase33u.a
            ${WX_INSTALL_DIR}/lib/gcc_x64_lib/wxtiff.a
            ${WX_INSTALL_DIR}/lib/gcc_x64_lib/wxjpeg.a
            ${WX_INSTALL_DIR}/lib/gcc_x64_lib/wxpng.a
            ${WX_INSTALL_DIR}/lib/gcc_x64_lib/wxzlib.a
            ${WX_INSTALL_DIR}/lib/gcc_x64_lib/wxregexu.a
            ${WX_INSTALL_DIR}/lib/gcc_x64_lib/wxexpat.a
            comctl32 uuid uxtheme version shlwapi gdi32 gdiplus msimg32 oleacc rpcrt4 winmm oleaut32 imm32
        )
        message(STATUS "Librerías wxWidgets configuradas")
        add_subdirectory(src/presentation)
    # Tercer intento: directorio alternativo (dentro de build/)
    elseif(EXISTS "${WX_INSTALL_DIR2}/include/wx/wx.h")
        message(STATUS "wxWidgets encontrado en: ${WX_INSTALL_DIR2}")
        set(wxWidgets_ROOT_DIR ${WX_INSTALL_DIR2})
        set(wxWidgets_INCLUDE_DIRS ${WX_INSTALL_DIR2}/include)
        set(wxWidgets_LIBRARY_DIRS ${WX_INSTALL_DIR2}/lib/gcc_x64_lib)
        add_subdirectory(src/presentation)
    # Cuarto intento: directorio descargado pero sin compilar
    elseif(EXISTS "${WX_DOWNLOAD_DIR}/include/wx/wx.h")
        message(STATUS "wxWidgets encontrado en: ${WX_DOWNLOAD_DIR} (sin compilar)")
        message(WARNING "wxWidgets necesita compilación. Ejecuta: cd build/wxwidgets && cmake --build . && cmake --install .")
        set(BUILD_GUI OFF)
    else()
        message(STATUS "wxWidgets no encontrado. Compilando solo versión consola.")
        set(BUILD_GUI OFF)
    endif()
else()
    message(STATUS "GUI build disabled")
endif()

# Console version always built
add_subdirectory(src/app)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Install targets
if(BUILD_GUI AND TARGET TallerParametrico)
    install(TARGETS TallerParametrico
        RUNTIME DESTINATION bin
    )
endif()
if(TARGET TallerParametrico_Console)
    install(TARGETS TallerParametrico_Console
        RUNTIME DESTINATION bin
    )
endif()
