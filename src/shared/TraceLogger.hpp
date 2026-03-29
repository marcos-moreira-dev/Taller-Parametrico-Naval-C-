#pragma once

/**
 * @file TraceLogger.hpp
 * @brief Sistema de trazabilidad avanzada
 * @author Proyecto Taller Paramétrico
 * @version 0.2.3
 * 
 * Sistema de trazabilidad avanzado con:
 * - Múltiples canales de log (tags)
 * - Filtrado por nivel
 * - Exportación a archivo
 * - callbacks para eventos
 * 
 * @note Más avanzado que Logger básico
 * @see Logger para logging simple
 */

#include <string>
#include <vector>
#include <chrono>
#include <fstream>
#include <mutex>
#include <map>
#include <functional>

namespace tp::shared {

// Niveles de trazabilidad
enum class TraceLevel {
    DEBUG,      // Información detallada para desarrollo
    INFO,       // Información general del flujo
    WARNING,    // Advertencias
    ERROR,      // Errores
    CRITICAL    // Errores críticos
};

// Tipos de eventos trazables
enum class TraceEventType {
    USER_ACTION,        // Acción del usuario (click, tecla)
    SIMULATION_STEP,    // Paso de simulación
    STATE_CHANGE,       // Cambio de estado
    FILE_OPERATION,     // Operación de archivo
    CALCULATION,        // Cálculo matemático
    GUI_UPDATE,         // Actualización de UI
    ERROR_EVENT,        // Error
    PERFORMANCE         // Métricas de rendimiento
};

// Entrada de trazabilidad
struct TraceEntry {
    std::chrono::system_clock::time_point timestamp;
    TraceLevel level;
    TraceEventType eventType;
    std::string component;      // Qué componente (ej: "TheoryPanel", "Simulation")
    std::string action;         // Qué acción (ej: "loadConcept", "runStep")
    std::string details;        // Detalles JSON o descripción
    std::string userContext;    // Contexto del usuario
    std::string sessionId;      // ID de sesión
    std::thread::id threadId;   // ID de thread
    
    // Constructor por defecto
    TraceEntry() = default;
    
    // Constructor completo
    TraceEntry(TraceLevel lvl, TraceEventType type, 
               const std::string& comp, const std::string& act,
               const std::string& det = "", 
               const std::string& ctx = "");
    
    // Convertir a string para logging
    std::string toString() const;
    
    // Convertir a JSON
    std::string toJson() const;
};

// Filtro de trazabilidad
struct TraceFilter {
    TraceLevel minLevel = TraceLevel::DEBUG;
    std::vector<TraceEventType> eventTypes; // Vacío = todos
    std::vector<std::string> components;    // Vacío = todos
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    std::string sessionId;
    
    TraceFilter() {
        startTime = std::chrono::system_clock::time_point::min();
        endTime = std::chrono::system_clock::time_point::max();
    }
    
    bool matches(const TraceEntry& entry) const;
};

// Callback para eventos de traza
using TraceCallback = std::function<void(const TraceEntry&)>;

// Logger de trazabilidad principal
class TraceLogger {
public:
    // Singleton
    static TraceLogger& instance();
    
    // Configuración
    void setLogFile(const std::string& filename);
    void setMinLevel(TraceLevel level);
    void setMaxEntries(size_t max);
    void enableConsoleOutput(bool enable);
    void enableFileOutput(bool enable);
    
    // Logging de trazas
    void log(TraceLevel level, TraceEventType type,
             const std::string& component, const std::string& action,
             const std::string& details = "",
             const std::string& userContext = "");
    
    // Métodos convenientes
    void debug(TraceEventType type, const std::string& comp, 
               const std::string& action, const std::string& details = "");
    void info(TraceEventType type, const std::string& comp,
              const std::string& action, const std::string& details = "");
    void warning(TraceEventType type, const std::string& comp,
                 const std::string& action, const std::string& details = "");
    void error(TraceEventType type, const std::string& comp,
               const std::string& action, const std::string& details = "");
    void critical(TraceEventType type, const std::string& comp,
                    const std::string& action, const std::string& details = "");
    
    // Callbacks
    void addCallback(TraceCallback callback);
    void removeCallback(size_t index);
    
    // Consultas y análisis
    std::vector<TraceEntry> query(const TraceFilter& filter);
    std::vector<TraceEntry> getRecent(size_t count);
    std::vector<TraceEntry> getByComponent(const std::string& component);
    std::vector<TraceEntry> getBySession(const std::string& sessionId);
    
    // Estadísticas
    std::map<std::string, size_t> getComponentStats();
    std::map<TraceEventType, size_t> getEventTypeStats();
    std::map<TraceLevel, size_t> getLevelStats();
    
    // Exportar
    void exportToJson(const std::string& filename);
    void exportToCsv(const std::string& filename);
    
    // Control
    void clear();
    void flush();
    
    // Utilidades
    std::string generateSessionId();
    std::string getCurrentTimestamp() const;
    
private:
    TraceLogger();
    ~TraceLogger();
    
    TraceLogger(const TraceLogger&) = delete;
    TraceLogger& operator=(const TraceLogger&) = delete;
    
    void writeToFile(const TraceEntry& entry);
    void writeToConsole(const TraceEntry& entry);
    void notifyCallbacks(const TraceEntry& entry);
    void rotateLogIfNeeded();
    
    std::vector<TraceEntry> entries_;
    std::ofstream fileStream_;
    std::mutex mutex_;
    std::vector<TraceCallback> callbacks_;
    
    // Configuración
    TraceLevel minLevel_ = TraceLevel::DEBUG;
    size_t maxEntries_ = 10000;
    bool consoleOutput_ = true;
    bool fileOutput_ = false;
    std::string logFilename_;
    std::string currentSessionId_;
    size_t maxFileSize_ = 10 * 1024 * 1024; // 10MB
};

// Macros convenientes para logging
#define TRACE_DEBUG(type, comp, action, ...) \
    tp::shared::TraceLogger::instance().debug(type, comp, action, ##__VA_ARGS__)

#define TRACE_INFO(type, comp, action, ...) \
    tp::shared::TraceLogger::instance().info(type, comp, action, ##__VA_ARGS__)

#define TRACE_WARNING(type, comp, action, ...) \
    tp::shared::TraceLogger::instance().warning(type, comp, action, ##__VA_ARGS__)

#define TRACE_ERROR(type, comp, action, ...) \
    tp::shared::TraceLogger::instance().error(type, comp, action, ##__VA_ARGS__)

#define TRACE_CRITICAL(type, comp, action, ...) \
    tp::shared::TraceLogger::instance().critical(type, comp, action, ##__VA_ARGS__)

// Helper para trazar ámbito (RAII)
class TraceScope {
public:
    TraceScope(const std::string& component, const std::string& action);
    ~TraceScope();
    
    void addDetail(const std::string& key, const std::string& value);
    
private:
    std::string component_;
    std::string action_;
    std::chrono::high_resolution_clock::time_point start_;
    std::map<std::string, std::string> details_;
};

#define TRACE_SCOPE(comp, action) \
    tp::shared::TraceScope traceScope_##__LINE__(comp, action)

} // namespace tp::shared
