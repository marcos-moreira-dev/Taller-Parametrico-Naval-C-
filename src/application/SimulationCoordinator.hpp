#pragma once

/**
 * @file SimulationCoordinator.hpp
 * @brief Coordinador de simulación - Patrón Mediator
 * @author Proyecto Taller Paramétrico
 * @version 1.1.0
 * 
 * @section DESCRIPTION
 * 
 * Este archivo implementa el patrón Mediator (Mediador) para la simulación.
 * El coordinador actúa como intermediario entre:
 * - La UI (MainWindow) que solicita acciones
 * - El SimulationRunner que ejecuta física
 * - Los callbacks que actualizan la UI con resultados
 * 
 * PATRÓN MEDIATOR:
 * @code
 * ┌─────────────────────────────────────────────────────────────┐
 * │                     SimulationCoordinator                    │
 * │                      (El Mediador)                           │
 * │                          │                                   │
 * │         ┌────────────────┼────────────────┐                  │
 * │         │                │                │                  │
 * │         ▼                ▼                ▼                  │
 * │   MainWindow    SimulationRunner    Callbacks/UI            │
 * │   (solicita)      (ejecuta)         (actualizan)            │
 * └─────────────────────────────────────────────────────────────┘
 * @endcode
 * 
 * BENEFICIOS DEL PATRÓN:
 * 1. Desacopla UI de física: No se conocen directamente
 * 2. Centraliza control: Un solo lugar gestiona el ciclo de vida
 * 3. Facilita testing: Podemos probar la coordinación sin GUI
 * 4. Flexibilidad: Fácil cambiar implementación de física o UI
 * 
 * @section STATE_MACHINE
 * 
 * La simulación tiene una máquina de estados bien definida:
 * 
 * @code
 *                            ┌──────────┐
 *            ┌──────────────►│ STOPPED  │◄──────────────┐
 *            │               │(inicial) │               │
 *            │               └────┬─────┘               │
 *            │                    │ start()             │ stop()
 *            │                    ▼                     │
 *            │            ┌──────────────┐              │
 *            │     ┌─────►│   RUNNING    │──────┐       │
 *            │     │      │ (ejecutando) │      │       │
 *            │     │      └──────┬───────┘      │       │
 *            │  pause()          │              │       │
 *            │     │             │ step()       │       │
 *            │     │             │              │       │
 *            │     │             ▼              │       │
 *            │     │      ┌──────────────┐      │       │
 *            │     └──────┤   PAUSED     │      │       │
 *            │            │(suspendida)  │──────┘       │
 *            │            └──────────────┘   resume()   │
 *            │                                          │
 *            │                    ┌──────────┐          │
 *            └────────────────────┤ FINISHED │──────────┘
 *                                 │(terminó) │
 *                                 └──────────┘
 * @endcode
 * 
 * TRANSICIONES VÁLIDAS:
 * - STOPPED → RUNNING: start()
 * - RUNNING → PAUSED: pause()
 * - PAUSED → RUNNING: pause() (toggle) o resume()
 * - RUNNING → STOPPED: stop()
 * - RUNNING → FINISHED: tiempo máximo alcanzado o colisión
 * 
 * @section CALLBACKS
 * 
 * El coordinador usa callbacks (patrón Observer) para notificar cambios:
 * 
 * 1. StateCallback: Notifica cambios de estado (Running → Paused, etc.)
 * 2. UpdateCallback: Notifica cada frame con el estado físico actual
 * 
 * Ejemplo de uso:
 * @code
 * SimulationCoordinator coordinator;
 * 
 * // Registrar callback para cambios de estado
 * coordinator.registerStateCallback([](SimulationState state) {
 *     if (state == SimulationState::Finished) {
 *         std::cout << "Simulación completada!" << std::endl;
 *     }
 * });
 * 
 * // Registrar callback para actualizaciones de frame
 * coordinator.registerUpdateCallback([](const PhysicsState& state) {
 *     updateGraph(state.velocity, state.energy);
 * });
 * 
 * // Inicializar y comenzar
 * coordinator.initialize(config);
 * coordinator.start();
 * 
 * // En loop principal (timer):
 * while (coordinator.getCurrentState() == SimulationState::Running) {
 *     coordinator.step();
 * }
 * @endcode
 * 
 * @note Este es el ÚNICO coordinador de simulación. No crear clases similares
 *       como SimulationController para evitar redundancia.
 * 
 * @see SimulationCoordinator.cpp para implementación detallada
 * @see ExperimentService para orquestación de alto nivel
 * @see SimulationRunner para el motor de física
 */

#include "ExperimentService.hpp"
#include "runner/SimulationRunner.hpp"
#include <functional>
#include <vector>

namespace tp::application {

/**
 * @defgroup simulation_coordinator Coordinador de Simulación
 * @brief Componentes para gestión del ciclo de vida de simulaciones
 * @{
 */

/**
 * @brief Estados posibles de la simulación
 * @details Representa la máquina de estados finitos de la simulación.
 *          Cada estado tiene transiciones válidas definidas.
 */
enum class SimulationState {
    Stopped,   ///< Estado inicial o después de stop(). No se ejecuta física.
    Running,   ///< Ejecutando física. step() avanza la simulación.
    Paused,    ///< Temporalmente detenida. step() no avanza pero mantiene estado.
    Finished   ///< Completada (tiempo máximo o colisión). Requiere reset() para reiniciar.
};

/**
 * @brief Punto de trayectoria para análisis y visualización
 * @details Almacena el estado completo del sistema en un instante de tiempo.
 *          Se usa para: gráficas, exportación a CSV, análisis post-simulación.
 */
struct TrajectoryPoint {
    double x;        ///< Posición X del bote [m]
    double y;        ///< Posición Y del bote [m]
    double time;     ///< Tiempo de simulación [s]
    double velocity; ///< Velocidad instantánea [m/s]
    double energy;   ///< Energía cinética total [J]
    
    /**
     * @brief Constructor por defecto
     * @details Inicializa todas las coordenadas a cero.
     *          Evita valores indeterminados en estructuras.
     */
    TrajectoryPoint() : x(0), y(0), time(0), velocity(0), energy(0) {}
    
    /**
     * @brief Constructor con valores
     * @param x_ Posición X
     * @param y_ Posición Y
     * @param t_ Tiempo
     * @param v_ Velocidad
     * @param e_ Energía
     */
    TrajectoryPoint(double x_, double y_, double t_, double v_, double e_)
        : x(x_), y(y_), time(t_), velocity(v_), energy(e_) {}
};

/**
 * @brief Estadísticas agregadas de la simulación
 * @details Métricas calculadas sobre toda la trayectoria.
 *          Se calculan al finalizar o cuando se solicitan.
 */
struct SimulationStats {
    double totalTime = 0.0;      ///< Duración total de la simulación [s]
    double totalDistance = 0.0;  ///< Distancia recorrida (integral de trayectoria) [m]
    double maxSpeed = 0.0;       ///< Velocidad máxima alcanzada [m/s]
    double averageSpeed = 0.0;   ///< Velocidad promedio (distancia/tiempo) [m/s]
    double finalEnergy = 0.0;    ///< Energía cinética final [J]
    double netDisplacement = 0.0;      ///< Desplazamiento neto desde inicio [m]
    double routeEfficiency = 0.0;      ///< Eficiencia (desplazamiento/distancia)
    bool endedByCollision = false;     ///< Indica si terminó por colisión
};

/**
 * @brief Callback para notificación de cambios de estado
 * @param newState El nuevo estado al que transicionó la simulación
 * 
 * @details Este callback se invoca SINCRÓNICAMENTE cuando cambia el estado.
 *          El receptor debe manejarlo rápidamente para no bloquear la simulación.
 * 
 * Ejemplo de uso:
 * @code
 * auto onStateChanged = [](SimulationState state) {
 *     switch (state) {
 *         case SimulationState::Running:
 *             ui->setStatus("Ejecutando...");
 *             break;
 *         case SimulationState::Paused:
 *             ui->setStatus("Pausado");
 *             break;
 *         case SimulationState::Finished:
 *             ui->setStatus("Completado");
 *             ui->showResults();
 *             break;
 *     }
 * };
 * @endcode
 */
using StateCallback = std::function<void(SimulationState)>;

/**
 * @brief Callback para notificación de actualizaciones de frame
 * @param state Estado físico actual del sistema
 * 
 * @details Este callback se invoca después de cada paso de integración exitoso.
 *          Úsalo para actualizar gráficas, canvas, o métricas en tiempo real.
 * 
 * @note Este callback puede llamarse frecuentemente (ej: 30-60 veces/segundo).
 *       Mantén la implementación eficiente.
 * 
 * Ejemplo de uso:
 * @code
 * auto onUpdate = [](const simulation::PhysicsState& state) {
 *     // Actualizar gráfica de velocidad
 *     velocityGraph->addPoint(state.time, state.velocity.magnitude());
 *     
 *     // Actualizar posición del bote en canvas
 *     canvas->setBoatPosition(state.position.x, state.position.y);
 * };
 * @endcode
 */
using UpdateCallback = std::function<void(const simulation::PhysicsState&)>;

/**
 * @brief Coordinador central de simulación
 * @class SimulationCoordinator
 * 
 * RESPONSABILIDADES:
 * 1. Gestionar el ciclo de vida de la simulación (start, pause, stop, reset)
 * 2. Coordinar entre UI y motor de física (SimulationRunner)
 * 3. Mantener historial de trayectoria para análisis
 * 4. Calcular estadísticas agregadas
 * 5. Notificar cambios mediante callbacks
 * 
 * RELACIONES:
 * - Usa: SimulationRunner (para cálculos físicos)
 * - Usa: ExperimentConfig (para configuración inicial)
 * - Observado por: MainWindow (mediante callbacks)
 * 
 * THREAD SAFETY:
 * - Esta clase NO es thread-safe por diseño
 * - wxWidgets es single-threaded para UI
 * - Todos los métodos deben llamarse desde el thread de UI
 * 
 * EJEMPLO DE USO COMPLETO:
 * @code
 * // 1. Crear y configurar
 * SimulationCoordinator coordinator;
 * ExperimentConfig config = loadConfig();
 * coordinator.initialize(config);
 * 
 * // 2. Registrar callbacks
 * coordinator.registerStateCallback([](SimulationState s) {
 *     std::cout << "Estado cambió a: " << static_cast<int>(s) << std::endl;
 * });
 * 
 * coordinator.registerUpdateCallback([](const PhysicsState& state) {
 *     updateVisualization(state);
 * });
 * 
 * // 3. Ejecutar simulación
 * coordinator.start();
 * 
 * // 4. En loop de timer (ej: cada 33ms para 30 FPS)
 * void onTimer() {
 *     if (coordinator.getCurrentState() == SimulationState::Running) {
 *         if (!coordinator.step()) {
 *             // Simulación terminó (colisión o tiempo máximo)
 *             auto stats = coordinator.getStatistics();
 *             displayResults(stats);
 *         }
 *     }
 * }
 * 
 * // 5. Limpiar al terminar
 * coordinator.stop();
 * @endcode
 */
class SimulationCoordinator {
public:
    /**
     * @brief Constructor
     * @details Inicializa el coordinador en estado STOPPED con valores por defecto.
     *          No crea el SimulationRunner todavía (se hace en initialize()).
     * 
     * VALORES POR DEFECTO:
     * - Estado: Stopped
     * - TimeStep: 0.01s (100 FPS para integración)
     * - MaxTime: 10s
     * - Método: Euler
     */
    SimulationCoordinator();
    
    /**
     * @brief Destructor
     * @details Detiene la simulación si está corriendo y libera recursos.
     *          El unique_ptr destruye automáticamente el SimulationRunner.
     */
    ~SimulationCoordinator() = default;
    
    // ============================================================
    // CONFIGURACIÓN
    // ============================================================
    
    /**
     * @brief Inicializa el coordinador con una configuración
     * @param config Configuración completa del experimento
     * 
     * @details Este método:
     * 1. Copia la configuración (para poder resetear más tarde)
     * 2. Crea el SimulationRunner con los parámetros dados
     * 3. Configura el integrador según el método seleccionado
     * 4. Resetea el estado a STOPPED
     * 
     * @pre El coordinador debe estar en estado STOPPED
     * @post Estado = STOPPED, listo para start()
     * 
     * @note Si se llama mientras la simulación está corriendo,
     *       primero se detiene automáticamente.
     */
    void initialize(const ExperimentConfig& config);
    
    /**
     * @brief Cambia el método de integración numérica
     * @param method Método a usar (Euler, ImprovedEuler, RK4)
     * 
     * @details El cambio toma efecto inmediatamente. Si la simulación está
     *          corriendo, el próximo step() usa el nuevo método.
     * 
     * COMPARACIÓN DE MÉTODOS:
     * - Euler: Rápido pero impreciso (error O(h²)). Para exploración rápida.
     * - Improved Euler: Balance precisión/velocidad (error O(h³))
     * - RK4: Preciso pero más lento (error O(h⁵)). Para resultados finales.
     * 
     * @note No reinicia la simulación. Permite cambiar método "en caliente"
     *       para comparar resultados.
     */
    void setIntegrationMethod(IntegrationMethod method);
    
    // ============================================================
    // CONTROL DE SIMULACIÓN
    // ============================================================
    
    /**
     * @brief Inicia la simulación
     * 
     * @details Transiciones:
     * - STOPPED → RUNNING: Comienza desde el inicio
     * - FINISHED → RUNNING: Primero hace reset(), luego inicia
     * - RUNNING → RUNNING: No hace nada (idempotente)
     * 
     * @pre initialize() debe haberse llamado primero
     * @post Estado = RUNNING, isPaused = false
     * 
     * Notifica a todos los StateCallbacks registrados.
     */
    void start();
    
    /**
     * @brief Pausa o reanuda la simulación
     * 
     * @details Toggle: si está corriendo la pausa, si está pausada la reanuda.
     * - RUNNING → PAUSED: Detiene avance pero mantiene estado
     * - PAUSED → RUNNING: Reanuda desde donde estaba
     * - STOPPED/FINISHED → (no hace nada)
     * 
     * @post isPaused = !isPaused (si estaba en RUNNING o PAUSED)
     */
    void pause();
    
    /**
     * @brief Detiene la simulación completamente
     * 
     * @details Detiene el SimulationRunner y resetea el estado.
     * La trayectoria acumulada se mantiene para análisis.
     * 
     * @post Estado = STOPPED, isPaused = false
     * 
     * Notifica a todos los StateCallbacks registrados.
     */
    void stop();
    
    /**
     * @brief Reinicia la simulación a estado inicial
     * 
     * @details Hace stop(), luego:
     * - Resetea tiempo a 0
     * - Limpia trayectoria
     * - Recrea el SimulationRunner desde config_
     * - Estado = STOPPED
     * 
     * @post Estado = STOPPED, tiempo = 0, trayectoria vacía
     */
    void reset();
    
    /**
     * @brief Ejecuta un único paso de simulación
     * @return true si la simulación puede continuar, false si terminó
     * 
     * @details Avanza la simulación por un timeStep:
     * 1. Verifica que estado sea RUNNING y no PAUSED
     * 2. Ejecuta integración mediante SimulationRunner
     * 3. Actualiza tiempo actual
     * 4. Almacena punto en trayectoria
     * 5. Verifica condiciones de término (colisión, tiempo máximo)
     * 6. Notifica UpdateCallbacks
     * 7. Si terminó, notifica StateCallbacks y retorna false
     * 
     * @note Este método debe llamarse periódicamente (ej: desde timer)
     *       mientras la simulación está RUNNING.
     * 
     * @note Si el estado es PAUSED, retorna true pero no avanza.
     */
    bool step();
    
    /**
     * @brief Ejecuta la simulación completa hasta el final
     * 
     * @details Bloqueante: ejecuta step() repetidamente hasta que termine.
     * Útil para simulaciones batch o tests donde no se necesita UI.
     * 
     * @warning Este método BLOQUEA el thread hasta terminar.
     *          No usar en UI thread si quieres mantenerla responsiva.
     * 
     * Ejemplo de uso en batch:
     * @code
     * coordinator.initialize(config);
     * coordinator.start();
     * coordinator.runComplete();  // Bloquea aquí
     * auto stats = coordinator.getStatistics();
     * saveToFile(stats);
     * @endcode
     */
    void runComplete();
    
    // ============================================================
    // CALLBACKS
    // ============================================================
    
    /**
     * @brief Registra callback para cambios de estado
     * @param callback Función a llamar cuando cambie el estado
     * 
     * @details Los callbacks se almacenan en un vector y se invocan
     *          en el orden de registro. Puedes registrar múltiples callbacks.
     * 
     * @note El callback se invoca SINCRÓNICAMENTE durante start(), stop(),
     *       o cuando la simulación termina naturalmente.
     * 
     * @warning No llamar a métodos del coordinador desde el callback
     *          (riesgo de recursión infinita).
     */
    void registerStateCallback(StateCallback callback);
    
    /**
     * @brief Registra callback para actualizaciones de frame
     * @param callback Función a llamar después de cada step exitoso
     * 
     * @details Útil para actualizar UI en tiempo real.
     *          El callback recibe el estado físico completo.
     * 
     * @note Este callback puede llamarse frecuentemente (30-60 Hz).
     *          Mantén la implementación eficiente.
     */
    void registerUpdateCallback(UpdateCallback callback);
    
    // ============================================================
    // CONSULTAS
    // ============================================================
    
    /**
     * @brief Obtiene el estado actual de la simulación
     * @return Estado actual (Stopped, Running, Paused, Finished)
     */
    SimulationState getCurrentState() const { return currentState_; }
    
    /**
     * @brief Obtiene el tiempo actual de simulación
     * @return Tiempo transcurrido desde el inicio [segundos]
     * 
     * @note Este tiempo es acumulado: tiempo = Σ(timeStep) para cada step
     *       Puede diferir ligeramente del tiempo real de reloj debido a:
     *       - Pausas
     *       - Variaciones en tiempo de procesamiento
     */
    double getCurrentTime() const { return currentTime_; }
    
    /**
     * @brief Obtiene la trayectoria acumulada
     * @return Vector de puntos de trayectoria
     * 
     * @details La trayectoria se mantiene incluso después de stop().
     *          Se limpia solo en reset() o initialize().
     * 
     * @note El número de puntos puede crecer indefinidamente en simulaciones
     *       largas. Considerar limitar para uso en producción.
     */
    const std::vector<TrajectoryPoint>& getTrajectory() const { return trajectory_; }
    
    /**
     * @brief Calcula y obtiene estadísticas de la simulación
     * @return Estructura con estadísticas calculadas
     * 
     * @details Calcula métricas sobre toda la trayectoria:
     * - totalDistance: Suma de distancias entre puntos consecutivos
     * - maxSpeed: Máximo de velocidades registradas
     * - averageSpeed: totalDistance / totalTime
     * - netDisplacement: Distancia en línea recta desde inicio hasta fin
     * - routeEfficiency: netDisplacement / totalDistance (1.0 = línea recta)
     */
    SimulationStats getStatistics() const;
    
    /**
     * @brief Verifica si la simulación está en ejecución
     * @return true si estado es RUNNING y no está pausada
     * 
     * @details Conveniencia equivalente a:
     *          getCurrentState() == SimulationState::Running && !isPaused_
     */
    bool isRunning() const {
        return currentState_ == SimulationState::Running && !isPaused_;
    }
    
    /**
     * @brief Verifica si la simulación está pausada
     * @return true si está en estado RUNNING pero pausada
     */
    bool isPaused() const {
        return currentState_ == SimulationState::Running && isPaused_;
    }
    
private:
    /**
     * @brief Actualiza la configuración del runner
     * @details Aplica cambios de configuración al SimulationRunner actual.
     *          Llamado internamente cuando cambian parámetros.
     */
    void updateConfiguration();
    
    /**
     * @brief Calcula distancia entre dos puntos de trayectoria
     * @param p1 Primer punto
     * @param p2 Segundo punto
     * @return Distancia euclidiana
     */
    static double distanceBetween(const TrajectoryPoint& p1, const TrajectoryPoint& p2);
    
    // ============================================================
    // MEMBROS
    // ============================================================
    
    ExperimentConfig config_;  ///< Configuración guardada (para reset)
    
    /**
     * @brief Runner de simulación (propiedad exclusiva)
     * @details unique_ptr asegura que solo el coordinador lo destruya.
     *          Se recrea en initialize() y reset().
     */
    std::unique_ptr<simulation::SimulationRunner> runner_;
    
    SimulationState currentState_;  ///< Estado actual de la máquina de estados
    bool isPaused_;                 ///< Flag de pausa (solo válido en RUNNING)
    double currentTime_;            ///< Tiempo acumulado de simulación [s]
    double timeStep_;               ///< Paso de tiempo para integración [s]
    double maxTime_;                ///< Tiempo máximo antes de auto-terminar [s]
    IntegrationMethod currentMethod_;  ///< Método de integración actual
    
    std::vector<TrajectoryPoint> trajectory_;  ///< Historial de posiciones
    
    /**
     * @brief Posición inicial del bote
     * @details Guardada para calcular desplazamiento neto.
     */
    double initialBoatX_ = 0.0;
    double initialBoatY_ = 0.0;
    
    std::vector<StateCallback> stateCallbacks_;   ///< Callbacks de cambio de estado
    std::vector<UpdateCallback> updateCallbacks_; ///< Callbacks de actualización
};

/** @} */  // end of simulation_coordinator

} // namespace tp::application
