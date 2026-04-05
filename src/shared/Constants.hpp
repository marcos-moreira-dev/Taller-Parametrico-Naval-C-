#pragma once

/**
 * @file Constants.hpp
 * @brief Constantes y configuración centralizada del sistema
 * @author Proyecto Taller Paramétrico
 * @version 1.0.0
 * 
 * @section PHILOSOPHY
 * 
 * Este archivo centraliza TODOS los valores constantes del sistema.
 * ¿Por qué es importante esto?
 * 
 * PROBLEMA DE "MAGIC NUMBERS":
 * @code
 * // ANTES (mal):
 * timer_->Start(33);  // ¿Qué es 33? ¿Milisegundos? ¿Segundos? ¿Por qué 33?
 * if (fieldVec.magnitude() > 0.1) {  // ¿Por qué 0.1?
 *     scale_ = std::min(scaleX, scaleY) * 0.8;  // ¿Por qué 0.8?
 * }
 * @endcode
 * 
 * SOLUCIÓN CON CONSTANTES NOMBRADAS:
 * @code
 * // DESPUÉS (bien):
 * timer_->Start(DEFAULT_FRAME_TIME_MS);  // Tiempo por frame a 30 FPS
 * if (fieldVec.magnitude() > AUTO_THRUST_MIN_FIELD_MAGNITUDE) {
 *     scale_ = std::min(scaleX, scaleY) * VIEWPORT_MARGIN_FACTOR;
 * }
 * @endcode
 * 
 * BENEFICIOS:
 * 1. Legibilidad: El nombre explica el propósito
 * 2. Mantenibilidad: Cambiar en un solo lugar
 * 3. Consistencia: Mismo valor en todo el código
 * 4. Documentación: Los nombres documentan intención
 * 
 * @note Este archivo NO debe tener dependencias de otros módulos para
 *       permitir su inclusión desde cualquier parte del sistema.
 * 
 * @see https://en.wikipedia.org/wiki/Magic_number_(programming)
 * @see https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rf-magic
 */

#include <cmath>  // Para M_PI (aunque en Windows puede necesitar _USE_MATH_DEFINES)

namespace tp::constants {

/* ============================================================================
 * SECCIÓN: Configuración de Simulación
 * ============================================================================
 * 
 * Estos valores controlan el comportamiento de la simulación física.
 * Cambiarlos afecta la precisión vs. rendimiento.
 */

/**
 * @defgroup simulation_config Configuración de Simulación
 * @brief Parámetros que controlan la simulación física
 * @{
 */

/**
 * @brief Tiempo por frame por defecto [milisegundos]
 * @details 33ms = ~30 FPS (frames por segundo)
 * 
 * Este valor determina la frecuencia de actualización de la simulación.
 * - Menor = más fluido pero más carga de CPU
 * - Mayor = menos fluido pero más eficiente
 * 
 * FÓRMULA: 1000ms / FPS = ms por frame
 * - 60 FPS = 16.67ms
 * - 30 FPS = 33.33ms  <- Este valor
 * - 15 FPS = 66.67ms
 */
inline constexpr double DEFAULT_FRAME_TIME_MS = 33.0;

/**
 * @brief Paso de tiempo por defecto para integración [segundos]
 * @details 0.033s = mismo que DEFAULT_FRAME_TIME_MS pero en segundos
 * 
 * Este es el Δt (delta tiempo) usado en las ecuaciones de integración.
 * Ver integrators/Integrators.hpp para las fórmulas específicas.
 * 
 * ESTABILIDAD NUMÉRICA:
 * - Euler: Δt debe ser pequeño (<0.05) para estabilidad
 * - Euler Mejorado: Puede ser un poco mayor
 * - RK4: Puede ser mayor gracias a mejor precisión (O(h⁵))
 */
inline constexpr double DEFAULT_TIME_STEP_S = 0.033;

/**
 * @brief Tiempo máximo de simulación por defecto [segundos]
 * @details 60 segundos = 1 minuto
 * 
 * Evita que simulaciones "escapen" infinitamente si hay un bug
 * o si el bote se atasca en un bucle.
 */
inline constexpr double DEFAULT_MAX_SIMULATION_TIME_S = 60.0;

/**
 * @brief Magnitud mínima del campo para Auto-Thrust
 * @details Si |campo| < este valor, no se ajusta dirección automáticamente
 * 
 * RAZÓN: Evita oscilaciones cuando el campo es casi cero (ríos muy lentos).
 *        También previene divisiones por números muy pequeños.
 */
inline constexpr double AUTO_THRUST_MIN_FIELD_MAGNITUDE = 0.1;

/** @} */  // end of simulation_config

/* ============================================================================
 * SECCIÓN: Configuración de Visualización
 * ============================================================================
 * 
 * Estos valores controlan cómo se renderiza la simulación.
 */

/**
 * @defgroup visualization_config Configuración de Visualización
 * @brief Parámetros de renderizado y UI
 * @{
 */

/**
 * @brief Factor de margen para viewport [adimensional]
 * @details 0.8 = dejar 20% de margen alrededor del escenario
 * 
 * Se usa para calcular la escala automática del canvas:
 * scale = min(scaleX, scaleY) * VIEWPORT_MARGIN_FACTOR
 */
inline constexpr double VIEWPORT_MARGIN_FACTOR = 0.8;

/**
 * @brief Margen en píxeles alrededor del escenario
 * @details 40px de padding en cada lado
 */
inline constexpr int CANVAS_PADDING_PX = 40;

/**
 * @brief Escala mínima permitida [píxeles por metro]
 * @details Evita zoom excesivo que haría el bote invisible
 */
inline constexpr double MIN_SCALE_PX_PER_M = 2.0;

/**
 * @brief Escala máxima permitida [píxeles por metro]
 * @details Evita zoom excesivo que mostraría solo una celda
 */
inline constexpr double MAX_SCALE_PX_PER_M = 50.0;

/**
 * @brief Escala por defecto [píxeles por metro]
 * @details Valor inicial antes de auto-ajuste
 */
inline constexpr double DEFAULT_SCALE_PX_PER_M = 10.0;

/** @} */  // end of visualization_config

/* ============================================================================
 * SECCIÓN: Límites del Sistema
 * ============================================================================
 * 
 * Estos valores definen los límites máximos de recursos.
 */

/**
 * @defgroup system_limits Límites del Sistema
 * @brief Restricciones de recursos y capacidades
 * @{
 */

/**
 * @brief Máximo número de partículas de agua
 * @details 800 partículas = balance visual/performance
 * 
 * Cada partícula consume memoria y tiempo de CPU.
 * - Más partículas = efectos más realistas pero más lento
 * - Menos partículas = mejor performance pero menos visual
 */
inline constexpr int MAX_WATER_PARTICLES = 800;

/**
 * @brief Máximo número de puntos de trayectoria a renderizar
 * @details Solo los últimos 1000 puntos se dibujan
 * 
 * La trayectoria completa se mantiene en memoria para cálculos,
 * pero solo se renderizan los últimos N puntos para rendimiento.
 */
inline constexpr int MAX_TRAJECTORY_RENDER_POINTS = 1000;

/**
 * @brief Tamaño máximo del escenario [celdas]
 * @details 200x200 = 40,000 celdas
 * 
 * Limita el uso de memoria y el tiempo de renderizado.
 * Cada celda usa 1 byte (enum class CellType).
 */
inline constexpr int MAX_SCENARIO_SIZE_CELLS = 200;

/**
 * @brief Tamaño mínimo del escenario [celdas]
 * @details 10x10 = escenario mínimo útil
 */
inline constexpr int MIN_SCENARIO_SIZE_CELLS = 10;

/**
 * @brief Historial máximo de comandos undo
 * @details 50 comandos = ~25 operaciones undo/redo
 * 
 * Balance entre memoria y utilidad para el usuario.
 */
inline constexpr int MAX_UNDO_HISTORY = 50;

/** @} */  // end of system_limits

/* ============================================================================
 * SECCIÓN: Constantes Matemáticas
 * ============================================================================
 * 
 * Estos valores son constantes matemáticas universales.
 */

/**
 * @defgroup math_constants Constantes Matemáticas
 * @brief Valores matemáticos comúnmente usados
 * @{
 */

/**
 * @brief Valor de PI con doble precisión
 * @details Definido explícitamente porque M_PI no es estándar C++
 * 
 * @note Windows requiere _USE_MATH_DEFINES antes de cmath
 * @note En C++20 se puede usar std::numbers::pi
 */
inline constexpr double PI = 3.14159265358979323846;

/**
 * @brief 2 * PI (frecuencia angular completa)
 */
inline constexpr double TWO_PI = 2.0 * PI;

/**
 * @brief PI / 2 (90 grados)
 */
inline constexpr double PI_HALF = PI / 2.0;

/**
 * @brief Valor pequeño para comparaciones de punto flotante
 * @details Usado para evitar problemas de precisión en comparaciones
 * 
 * @code
 * // En lugar de: if (x == 0.0)
 * if (std::abs(x) < EPSILON)  // Comparación segura
 * @endcode
 */
inline constexpr double EPSILON = 1e-9;

/**
 * @brief Factor de conversión: grados a radianes
 * @details radianes = grados * DEG_TO_RAD
 */
inline constexpr double DEG_TO_RAD = PI / 180.0;

/**
 * @brief Factor de conversión: radianes a grados
 * @details grados = radianes * RAD_TO_DEG
 */
inline constexpr double RAD_TO_DEG = 180.0 / PI;

/** @} */  // end of math_constants

/* ============================================================================
 * SECCIÓN: Constantes de Física
 * ============================================================================
 * 
 * Estos valores son constantes físicas usadas en simulaciones.
 */

/**
 * @defgroup physics_constants Constantes Físicas
 * @brief Valores físicos del mundo real
 * @{
 */

/**
 * @brief Densidad del agua dulce [kg/m³]
 * @details Aproximadamente 1000 kg/m³ a temperatura ambiente
 */
inline constexpr double WATER_DENSITY_KG_M3 = 1000.0;

/**
 * @brief Densidad del agua salada [kg/m³]
 * @details Aproximadamente 1025 kg/m³ (agua de mar)
 */
inline constexpr double SEA_WATER_DENSITY_KG_M3 = 1025.0;

/**
 * @brief Gravedad estándar [m/s²]
 * @details Valor estándar a nivel del mar
 */
inline constexpr double GRAVITY_M_S2 = 9.81;

/**
 * @brief Velocidad del sonido en el aire [m/s]
 * @details Aproximadamente 343 m/s a 20°C
 */
inline constexpr double SPEED_OF_SOUND_M_S = 343.0;

/** @} */  // end of physics_constants

/* ============================================================================
 * SECCIÓN: Valores por Defecto de Entidades
 * ============================================================================
 * 
 * Valores iniciales para botes, motores, etc.
 */

/**
 * @defgroup entity_defaults Valores por Defecto de Entidades
 * @brief Configuración inicial de objetos del dominio
 * @{
 */

/**
 * @brief Masa por defecto del bote [kg]
 * @details 10 kg = bote pequeño de modelo
 */
inline constexpr double DEFAULT_BOAT_MASS_KG = 10.0;

/**
 * @brief Tamaño por defecto del bote [metros]
 * @details 5m = bote pequeño realista
 */
inline constexpr double DEFAULT_BOAT_SIZE_M = 5.0;

/**
 * @brief Coeficiente de arrastre por defecto
 * @details 0.5 = valor típico para formas aerodinámicas simples
 */
inline constexpr double DEFAULT_DRAG_COEFFICIENT = 0.5;

/**
 * @brief Posición inicial X del bote [metros]
 * @details Centro del escenario por defecto (asumiendo 50x50)
 */
inline constexpr double DEFAULT_BOAT_POS_X_M = 25.0;

/**
 * @brief Posición inicial Y del bote [metros]
 * @details Centro del escenario por defecto
 */
inline constexpr double DEFAULT_BOAT_POS_Y_M = 25.0;

/**
 * @brief Empuje máximo del motor [Newtons]
 * @details 100 N = fuerza razonable para bote pequeño
 */
inline constexpr double DEFAULT_MAX_MOTOR_THRUST_N = 100.0;

/** @} */  // end of entity_defaults

/* ============================================================================
 * SECCIÓN: Configuración de Archivos
 * ============================================================================
 */

/**
 * @defgroup file_config Configuración de Archivos
 * @brief Formatos y extensiones de archivos
 * @{
 */

/**
 * @brief Extensión de archivos de escenario
 */
inline constexpr const char* SCENARIO_FILE_EXTENSION = ".scenario";

/**
 * @brief Extensión de archivos CSV de exportación
 */
inline constexpr const char* CSV_FILE_EXTENSION = ".csv";

/**
 * @brief Extensión de archivos PNG de exportación
 */
inline constexpr const char* PNG_FILE_EXTENSION = ".png";

/**
 * @brief Versión actual del formato de archivo de escenario
 */
inline constexpr int SCENARIO_FILE_FORMAT_VERSION = 1;

/** @} */  // end of file_config

} // namespace tp::constants
