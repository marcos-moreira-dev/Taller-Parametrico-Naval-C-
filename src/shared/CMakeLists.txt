# @file CMakeLists.txt
# @brief Configuración de construcción para el módulo 'shared'
# @details Este módulo contiene utilidades compartidas entre todas las capas
#          del sistema. Incluye logging básico y trazabilidad avanzada.
#
# @note Es importante mantener este módulo sin dependencias externas para
#       permitir su uso desde cualquier otra capa (domain, simulation, etc.)
#
# ESTRUCTURA DEL MÓDULO:
#   - Logger.cpp: Logging básico con niveles (DEBUG, INFO, WARNING, ERROR)
#   - TraceLogger.cpp: Trazabilidad avanzada para debugging y auditoría
#
# Como este es un módulo de CMake (no C++), usamos comentarios con #
# en lugar de /** */ que solo funcionan en código C++

add_library(tp_shared
    Logger.cpp              # Logging básico con niveles de severidad
    TraceLogger.cpp         # Trazabilidad avanzada de eventos del sistema
                            # (simulación, UI, operaciones de archivo, etc.)
)

target_include_directories(tp_shared 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(tp_shared PUBLIC cxx_std_20)
