/**
 * @file SimulationCoordinator.cpp
 * @brief Implementación del Coordinador de Simulación
 * @author Proyecto Taller Paramétrico
 * @version 1.1.0
 * 
 * @section IMPLEMENTATION_NOTES
 * 
 * Este archivo implementa la máquina de estados y coordinación detallada
 * en SimulationCoordinator.hpp. Ver el header para documentación de
 * arquitectura y patrones de diseño.
 * 
 * DECISIONES DE IMPLEMENTACIÓN:
 * 
 * 1. MÁQUINA DE ESTADOS:
 *    - Usamos enum class SimulationState en lugar de booleanos múltiples
 *    - Esto previene estados inválidos (ej: Running + Stopped simultáneamente)
 *    - Las transiciones están encapsuladas en métodos (start, pause, stop)
 * 
 * 2. CALLBACKS:
 *    - Almacenamos std::function en vectores (permite múltiples observadores)
 *    - Invocamos en el orden de registro
 *    - No manejamos excepciones de callbacks (asumimos que no lanzan)
 * 
 * 3. TRAYECTORIA:
 *    - Almacenamos todos los puntos (podría crecer mucho en simulaciones largas)
 *    - Para producción, considerar limitar tamaño o usar ring buffer
 *    - Por ahora, es aceptable para un taller educativo
 * 
 * 4. CÁLCULO DE MÉTRICAS:
 *    - Distancia total: suma de distancias euclidianas entre puntos
 *    - Desplazamiento neto: distancia en línea recta desde inicio hasta fin
 *    - Eficiencia: desplazamiento_net / distancia_total (1.0 = óptimo)
 * 
 * 5. THREADING:
 *    - No usamos mutex porque wxWidgets es single-threaded
 *    - Todos los métodos deben llamarse desde el thread de UI
 *    - step() es síncrono y bloqueante (pero rápido)
 */

#include "SimulationCoordinator.hpp"
#include "Logger.hpp"
#include <cmath>
#include <limits>

namespace tp::application {

/* ============================================================================
 * CONSTRUCTOR
 * ============================================================================ */

SimulationCoordinator::SimulationCoordinator()
    : currentState_(SimulationState::Stopped)
    , isPaused_(false)
    , currentTime_(0.0)
    , timeStep_(0.01)      // 100 Hz para integración precisa
    , maxTime_(10.0)       // 10 segundos por defecto
    , currentMethod_(IntegrationMethod::Euler)
    , initialBoatX_(0.0)
    , initialBoatY_(0.0) {
    /**
     * @internal INICIALIZACIÓN CONSERVADORA
     * 
     * Valores por defecto seguros:
     * - Estado STOPPED (no consume recursos)
     * - Runner no creado todavía (lazy initialization)
     * - Trayectoria vacía
     * 
     * El runner se crea en initialize() para permitir:
     * 1. Cambiar configuración antes de iniciar
     * 2. Reutilizar el coordinador para múltiples simulaciones
     * 3. Evitar crear recursos que quizás no se usen
     */
    LOG_INFO_MACRO("SimulationCoordinator inicializado");
}

/* ============================================================================
 * CONFIGURACIÓN
 * ============================================================================ */

void SimulationCoordinator::initialize(const ExperimentConfig& config) {
    /**
     * @internal INICIALIZACIÓN COMPLETA
     * 
     * PASOS:
     * 1. Guardar configuración (copia para poder resetear)
     * 2. Resetear estado
     * 3. Crear SimulationRunner
     * 4. Configurar integrador
     * 
     * PRECONDICIÓN: Puede llamarse en cualquier estado.
     * POSTCONDICIÓN: Estado = STOPPED, listo para start()
     */
    
    LOG_INFO_MACRO("Inicializando coordinador de simulación...");
    
    // Guardar configuración para reset()
    config_ = config;
    
    // Resetear estado
    currentTime_ = 0.0;
    currentState_ = SimulationState::Stopped;
    isPaused_ = false;
    trajectory_.clear();
    
    // Guardar posición inicial para cálculo de desplazamiento
    auto initialPos = config_.boat.getPosition();
    initialBoatX_ = initialPos.x;
    initialBoatY_ = initialPos.y;
    
    // Crear runner con la configuración inicial
    // Usamos make_unique (C++14) para crear y asignar en una línea
    runner_ = std::make_unique<simulation::SimulationRunner>(
        config_.boat, config_.field, config_.motor, config_.scenario
    );
    
    // Configurar parámetros del runner
    runner_->setTimeStep(config_.timeStep);
    runner_->setMaxTime(config_.maxTime);
    
    // Configurar integrador según el método seleccionado
    // Esto crea el integrador apropiado (Euler, ImprovedEuler, RK4)
    setIntegrationMethod(config_.method);
    
    LOG_INFO_MACRO("Coordinador inicializado correctamente");
}

void SimulationCoordinator::setIntegrationMethod(IntegrationMethod method) {
    /**
     * @internal CAMBIO DE INTEGRADOR (PATRÓN STRATEGY)
     * 
     * Usamos el patrón Strategy para cambiar algoritmos de integración
     * en tiempo de ejecución. Cada integrador implementa la misma interfaz
     * pero con diferente precisión/velocidad.
     * 
     * COMPARACIÓN:
     * - Euler: 1 evaluación de fuerza por paso (rápido, impreciso)
     * - Improved Euler: 2 evaluaciones (balance)
     * - RK4: 4 evaluaciones (lento, muy preciso)
     * 
     * La elección depende de:
     * - Precisión requerida (RK4 para resultados finales)
     * - Velocidad deseada (Euler para exploración rápida)
     * - Estabilidad del sistema (campos fuertes necesitan pasos pequeños)
     */
    
    currentMethod_ = method;
    
    if (!runner_) {
        // No hay runner todavía, solo guardamos el método
        // Se aplicará cuando se cree el runner
        return;
    }
    
    std::unique_ptr<simulation::Integrator> integrator;
    
    switch (method) {
        case IntegrationMethod::Euler:
            LOG_DEBUG_MACRO("Usando integrador Euler (O(h²))");
            integrator = std::make_unique<simulation::EulerIntegrator>();
            break;
            
        case IntegrationMethod::ImprovedEuler:
            LOG_DEBUG_MACRO("Usando integrador Euler Mejorado (O(h³))");
            integrator = std::make_unique<simulation::ImprovedEulerIntegrator>();
            break;
            
        case IntegrationMethod::RK4:
            LOG_DEBUG_MACRO("Usando integrador RK4 (O(h⁵))");
            integrator = std::make_unique<simulation::RK4Integrator>();
            break;
            
        default:
            LOG_WARNING("Método de integración desconocido, usando Euler por defecto");
            integrator = std::make_unique<simulation::EulerIntegrator>();
    }
    
    // Transferimos ownership al runner
    // El runner destruye el integrador anterior automáticamente
    runner_->setIntegrator(std::move(integrator));
}

/* ============================================================================
 * CONTROL DE SIMULACIÓN
 * ============================================================================ */

void SimulationCoordinator::start() {
    /**
     * @internal INICIO DE SIMULACIÓN
     * 
     * TRANSICIONES VÁLIDAS:
     * - STOPPED → RUNNING: Inicia desde el principio
     * - FINISHED → RUNNING: Primero hace reset(), luego inicia
     * - RUNNING → RUNNING: No hace nada (idempotente)
     * - PAUSED → RUNNING: Usar resume() en lugar de start()
     * 
     * @pre initialize() debe haberse llamado (runner_ != nullptr)
     * @post Estado = RUNNING, isPaused = false
     */
    
    if (currentState_ == SimulationState::Running) {
        LOG_WARNING("La simulación ya está en ejecución");
        return;
    }
    
    if (currentState_ == SimulationState::Finished) {
        // Si ya terminó, reiniciamos primero
        LOG_INFO_MACRO("Reiniciando simulación antes de iniciar...");
        reset();
    }
    
    currentState_ = SimulationState::Running;
    isPaused_ = false;
    
    LOG_INFO_MACRO("Simulación iniciada");
    
    // Notificar a todos los callbacks registrados
    // Usamos const& para evitar copias innecesarias
    for (const auto& callback : stateCallbacks_) {
        callback(currentState_);
    }
}

void SimulationCoordinator::pause() {
    /**
     * @internal PAUSA/REANUDACIÓN (TOGGLE)
     * 
     * Este método alterna entre pausado y corriendo.
     * Solo tiene efecto si el estado es RUNNING.
     * 
     * IMPLEMENTACIÓN:
     * - isPaused_ es un flag separado del estado
     * - Esto permite mantener el estado RUNNING mientras está pausada
     * - La ventaja es que step() puede verificar rápidamente
     * 
     * ALTERNATIVA CONSIDERADA:
     * - Usar estado PAUSED separado en lugar de flag
     * - Rechazado porque complica las transiciones (más estados = más casos)
     */
    
    if (currentState_ != SimulationState::Running) {
        // Solo pausar si está corriendo
        return;
    }
    
    isPaused_ = !isPaused_;
    
    if (isPaused_) {
        LOG_INFO_MACRO("Simulación pausada");
    } else {
        LOG_INFO_MACRO("Simulación reanudada");
    }
}

void SimulationCoordinator::stop() {
    /**
     * @internal DETENCIÓN DE SIMULACIÓN
     * 
     * Diferencia entre stop() y pause():
     * - stop(): Termina completamente, requiere start() para reiniciar
     * - pause(): Temporal, se reanuda con pause() o resume()
     * 
     * Este método:
     * 1. Detiene el runner
     * 2. Cambia estado a STOPPED
     * 3. Notifica callbacks
     * 
     * @post Estado = STOPPED, isPaused = false
     * @note La trayectoria se mantiene para análisis post-simulación
     */
    
    if (currentState_ == SimulationState::Stopped) {
        // Ya está detenida, no hacer nada
        return;
    }
    
    currentState_ = SimulationState::Stopped;
    isPaused_ = false;
    
    if (runner_) {
        runner_->stop();
    }
    
    LOG_INFO_MACRO("Simulación detenida");
    
    // Notificar a todos los callbacks
    for (const auto& callback : stateCallbacks_) {
        callback(currentState_);
    }
}

void SimulationCoordinator::reset() {
    /**
     * @internal REINICIO COMPLETO
     * 
     * Este método hace reset total:
     * 1. Detiene si está corriendo
     * 2. Limpia trayectoria y tiempo
     * 3. Reinicia el bote a posición inicial
     * 4. Recrea el runner (limpia estado interno)
     * 
     * @post Estado = STOPPED, tiempo = 0, trayectoria vacía
     * @post Bote en posición inicial
     */
    
    // Primero detenemos si es necesario
    stop();
    
    // Resetear métricas
    currentTime_ = 0.0;
    trajectory_.clear();
    currentState_ = SimulationState::Stopped;
    isPaused_ = false;
    
    // Reiniciar el bote a su posición inicial
    config_.boat.reset();
    
    // Recrear el runner para limpiar estado interno
    if (runner_) {
        runner_->reset();
    }
    
    LOG_INFO_MACRO("Simulación reiniciada");
}

bool SimulationCoordinator::step() {
    /**
     * @internal PASO DE SIMULACIÓN (CORE)
     * 
     * Este es el método más importante. Ejecuta un paso de integración:
     * 
     * ALGORITMO:
     * 1. Verificar precondiciones (RUNNING y no pausada)
     * 2. Actualizar configuración en tiempo real
     * 3. Ejecutar integración mediante runner
     * 4. Procesar resultado
     * 5. Verificar condiciones de término
     * 6. Notificar callbacks
     * 
     * RETORNO:
     * - true: La simulación puede continuar
     * - false: La simulación terminó (colisión o tiempo máximo)
     * 
     * @note Este método debe llamarse periódicamente desde un timer
     *       (ej: cada 33ms para 30 FPS)
     */
    
    // PRECONDICIONES
    if (currentState_ != SimulationState::Running || isPaused_) {
        // No podemos avanzar si no está corriendo o está pausada
        return false;
    }
    
    if (!runner_) {
        LOG_ERROR_MACRO("Runner no inicializado. ¿Se llamó initialize()?");
        return false;
    }
    
    // Actualizar configuración dinámica
    // Esto permite cambiar parámetros (ej: timestep) durante la simulación
    updateConfiguration();
    
    // EJECUTAR INTEGRACIÓN
    // El runner ejecuta la física y retorna el resultado
    auto result = runner_->run();
    
    // Verificar que tengamos estados
    if (result.states.empty()) {
        LOG_WARNING("El runner no retornó estados");
        return false;
    }
    
    // Tomar el último estado calculado
    const auto& state = result.states.back();
    currentTime_ = state.time;
    
    // GUARDAR TRAYECTORIA
    // Creamos un punto con toda la información relevante
    TrajectoryPoint point;
    point.x = state.position.x;
    point.y = state.position.y;
    point.time = state.time;
    point.velocity = state.velocity.magnitude();
    point.energy = state.kineticEnergy;
    trajectory_.push_back(point);
    
    // Actualizar el modelo del bote
    // Esto mantiene sincronizado el bote con la simulación
    config_.boat.setPosition(state.position);
    config_.boat.setVelocity(state.velocity);
    
    // VERIFICAR CONDICIONES DE TÉRMINO
    
    // 1. Colisión
    if (state.collision) {
        currentState_ = SimulationState::Finished;
        LOG_INFO_MACRO("Simulación finalizada por colisión");
        
        // Notificar callbacks de estado
        for (const auto& callback : stateCallbacks_) {
            callback(currentState_);
        }
        return false;
    }
    
    // 2. Tiempo máximo alcanzado
    if (currentTime_ >= maxTime_) {
        currentState_ = SimulationState::Finished;
        LOG_INFO_MACRO("Simulación finalizada (tiempo máximo alcanzado)");
        
        // Notificar callbacks de estado
        for (const auto& callback : stateCallbacks_) {
            callback(currentState_);
        }
        return false;
    }
    
    // NOTIFICAR CALLBACKS DE ACTUALIZACIÓN
    // Esto permite a la UI actualizar gráficas, canvas, etc.
    for (const auto& callback : updateCallbacks_) {
        callback(state);
    }
    
    return true;
}

void SimulationCoordinator::runComplete() {
    /**
     * @internal EJECUCIÓN COMPLETA (BLOQUEANTE)
     * 
     * Este método ejecuta la simulación completa de una vez.
     * Es BLOQUEANTE: no retorna hasta que termine.
     * 
     * USOS:
     * - Simulaciones batch (sin UI)
     * - Tests automatizados
     * - Cálculos en segundo plano
     * 
     * NO USAR:
     * - En UI thread de aplicaciones interactivas (congelaría la UI)
     * - Cuando se necesita actualización en tiempo real
     * 
     * @warning Este método bloquea el thread actual.
     */
    
    // Asegurar que esté corriendo
    if (currentState_ != SimulationState::Running) {
        start();
    }
    
    LOG_INFO_MACRO("Ejecutando simulación completa (bloqueante)...");
    
    // Ejecutar step() repetidamente hasta que termine
    // Esto es un loop simple pero efectivo
    while (step()) {
        // step() retorna false cuando termina
        // (colisión o tiempo máximo)
    }
    
    LOG_INFO_MACRO("Simulación completa finalizada");
}

/* ============================================================================
 * CALLBACKS
 * ============================================================================ */

void SimulationCoordinator::registerStateCallback(StateCallback callback) {
    /**
     * @internal REGISTRO DE CALLBACK DE ESTADO
     * 
     * Almacenamos el callback en un vector. Esto permite:
     * - Múltiples observadores (ej: MainWindow + Logger)
     * - Orden de ejecución predecible (orden de registro)
     * 
     * Usamos std::move para transferir ownership eficientemente.
     * Si el callback contiene capturas grandes (ej: [this]),
     * evitamos copias innecesarias.
     */
    stateCallbacks_.push_back(std::move(callback));
}

void SimulationCoordinator::registerUpdateCallback(UpdateCallback callback) {
    /**
     * @internal REGISTRO DE CALLBACK DE ACTUALIZACIÓN
     * 
     * Similar a registerStateCallback pero para actualizaciones de frame.
     * Estos callbacks se invocan más frecuentemente (cada step).
     * 
     * PERFORMANCE:
     * - Mantén los callbacks ligeros (evita trabajo pesado)
     * - No hagas I/O bloqueante (archivos, red) desde aquí
     * - Delega a workers si necesitas procesamiento pesado
     */
    updateCallbacks_.push_back(std::move(callback));
}

/* ============================================================================
 * CONSULTAS
 * ============================================================================ */

SimulationStats SimulationCoordinator::getStatistics() const {
    /**
     * @internal CÁLCULO DE ESTADÍSTICAS
     * 
     * Calculamos métricas derivadas de la trayectoria:
     * 
     * DISTANCIA TOTAL:
     * Sumamos las distancias euclidianas entre puntos consecutivos:
     * total = Σ(√((xᵢ - xᵢ₋₁)² + (yᵢ - yᵢ₋₁)²))
     * 
     * DESPLAZAMIENTO NETO:
     * Distancia en línea recta desde posición inicial hasta final:
     * net = √((x_final - x_inicial)² + (y_final - y_inicial)²)
     * 
     * EFICIENCIA DE RUTA:
     * Indica qué tan directo fue el camino:
     * efficiency = netDisplacement / totalDistance
     * - 1.0 = línea recta perfecta
     * - 0.5 = muchas desviaciones
     * - 0.0 = volvió al punto de partida
     * 
     * COMPLEJIDAD: O(n) donde n = número de puntos de trayectoria
     */
    
    SimulationStats stats;
    
    // Inicializar a cero
    stats.totalTime = currentTime_;
    stats.totalDistance = 0.0;
    stats.maxSpeed = 0.0;
    stats.averageSpeed = 0.0;
    stats.finalEnergy = 0.0;
    stats.netDisplacement = 0.0;
    stats.routeEfficiency = 0.0;
    stats.endedByCollision = (currentState_ == SimulationState::Finished && 
                              !trajectory_.empty() && 
                              currentTime_ >= maxTime_);
    
    // Si no hay trayectoria, retornar stats vacíos
    if (trajectory_.empty()) {
        return stats;
    }
    
    // Calcular estadísticas iterando sobre la trayectoria
    double totalDistance = 0.0;
    double maxSpeed = 0.0;
    
    for (size_t i = 1; i < trajectory_.size(); ++i) {
        // Distancia entre punto i-1 y punto i
        totalDistance += distanceBetween(trajectory_[i-1], trajectory_[i]);
        
        // Velocidad máxima
        if (trajectory_[i].velocity > maxSpeed) {
            maxSpeed = trajectory_[i].velocity;
        }
    }
    
    // Calcular desplazamiento neto (línea recta desde inicio hasta fin)
    const auto& finalPoint = trajectory_.back();
    double dx = finalPoint.x - initialBoatX_;
    double dy = finalPoint.y - initialBoatY_;
    double netDisplacement = std::sqrt(dx*dx + dy*dy);
    
    // Calcular eficiencia de ruta
    double efficiency = 0.0;
    if (totalDistance > 1e-9) {  // Evitar división por cero
        efficiency = netDisplacement / totalDistance;
    }
    
    // Llenar estructura de retorno
    stats.totalDistance = totalDistance;
    stats.maxSpeed = maxSpeed;
    stats.averageSpeed = (currentTime_ > 1e-9) ? totalDistance / currentTime_ : 0.0;
    stats.finalEnergy = trajectory_.back().energy;
    stats.netDisplacement = netDisplacement;
    stats.routeEfficiency = efficiency;
    
    return stats;
}

/* ============================================================================
 * MÉTODOS PRIVADOS
 * ============================================================================ */

void SimulationCoordinator::updateConfiguration() {
    /**
     * @internal ACTUALIZACIÓN DE CONFIGURACIÓN DINÁMICA
     * 
     * Este método actualiza parámetros que pueden cambiar en tiempo real.
     * Se llama antes de cada step() para permitir ajustes dinámicos.
     * 
     * EJEMPLO: Si el usuario cambia el timestep durante la simulación,
     *          este método aplica el cambio inmediatamente.
     * 
     * NOTA: Algunos parámetros requieren reinicio (ej: cambiar escenario).
     *       Este método solo maneja parámetros "hot-swappable".
     */
    
    if (!runner_) {
        return;
    }
    
    // Actualizar timestep
    runner_->setTimeStep(config_.timeStep);
    
    // Actualizar tiempo máximo (extendemos desde tiempo actual)
    // Esto permite simulaciones indefinidas si maxTime es muy grande
    runner_->setMaxTime(currentTime_ + config_.timeStep + 0.001);
}

double SimulationCoordinator::distanceBetween(
    const TrajectoryPoint& p1, 
    const TrajectoryPoint& p2
) {
    /**
     * @internal CÁLCULO DE DISTANCIA EUCLIDIANA
     * 
     * Fórmula: d = √((x₂-x₁)² + (y₂-y₁)²)
     * 
     * Esta función es estática porque no necesita acceso a miembros del objeto.
     * Esto mejora la encapsulación y permite optimizaciones del compilador.
     * 
     * @param p1 Primer punto
     * @param p2 Segundo punto
     * @return Distancia euclidiana entre los puntos
     */
    
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;
    return std::sqrt(dx*dx + dy*dy);
}

} // namespace tp::application
