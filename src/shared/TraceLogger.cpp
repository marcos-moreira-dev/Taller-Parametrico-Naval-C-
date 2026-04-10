#include "TraceLogger.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <ctime>

namespace tp::shared {

// TraceEntry implementation
TraceEntry::TraceEntry(TraceLevel lvl, TraceEventType type, 
                       const std::string& comp, const std::string& act,
                       const std::string& det, const std::string& ctx)
    : timestamp(std::chrono::system_clock::now())
    , level(lvl)
    , eventType(type)
    , component(comp)
    , action(act)
    , details(det)
    , userContext(ctx)
    , threadId(std::this_thread::get_id()) {
}

std::string TraceEntry::toString() const {
    std::ostringstream oss;
    
    // Timestamp
    auto time_t = std::chrono::system_clock::to_time_t(timestamp);
    auto tm = std::localtime(&time_t);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        timestamp.time_since_epoch()) % 1000;
    
    oss << std::put_time(tm, "%Y-%m-%d %H:%M:%S");
    oss << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    // Nivel
    switch (level) {
        case TraceLevel::DEBUG: oss << " [DEBUG]"; break;
        case TraceLevel::INFO: oss << " [INFO]"; break;
        case TraceLevel::WARNING: oss << " [WARN]"; break;
        case TraceLevel::ERR: oss << " [ERROR]"; break;
        case TraceLevel::CRITICAL: oss << " [CRIT]"; break;
    }
    
    // Componente y acción
    oss << " [" << component << "] " << action;
    
    // Detalles si existen
    if (!details.empty()) {
        oss << " - " << details;
    }
    
    return oss.str();
}

std::string TraceEntry::toJson() const {
    std::ostringstream oss;
    oss << "{";
    
    // Timestamp
    auto time_t = std::chrono::system_clock::to_time_t(timestamp);
    auto tm = std::gmtime(&time_t);
    char buf[30];
    std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", tm);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        timestamp.time_since_epoch()) % 1000;
    
    oss << "\"timestamp\":\"" << buf << "." << std::setfill('0') 
        << std::setw(3) << ms.count() << "Z\",";
    
    // Level
    oss << "\"level\":";
    switch (level) {
        case TraceLevel::DEBUG: oss << "\"DEBUG\""; break;
        case TraceLevel::INFO: oss << "\"INFO\""; break;
        case TraceLevel::WARNING: oss << "\"WARNING\""; break;
        case TraceLevel::ERR: oss << "\"ERROR\""; break;
        case TraceLevel::CRITICAL: oss << "\"CRITICAL\""; break;
    }
    oss << ",";
    
    // Event type
    oss << "\"eventType\":";
    switch (eventType) {
        case TraceEventType::USER_ACTION: oss << "\"USER_ACTION\""; break;
        case TraceEventType::SIMULATION_STEP: oss << "\"SIMULATION_STEP\""; break;
        case TraceEventType::STATE_CHANGE: oss << "\"STATE_CHANGE\""; break;
        case TraceEventType::FILE_OPERATION: oss << "\"FILE_OPERATION\""; break;
        case TraceEventType::CALCULATION: oss << "\"CALCULATION\""; break;
        case TraceEventType::GUI_UPDATE: oss << "\"GUI_UPDATE\""; break;
        case TraceEventType::ERROR_EVENT: oss << "\"ERROR_EVENT\""; break;
        case TraceEventType::PERFORMANCE: oss << "\"PERFORMANCE\""; break;
    }
    oss << ",";
    
    // Component y action
    oss << "\"component\":\"" << component << "\",";
    oss << "\"action\":\"" << action << "\",";
    
    // Details
    if (!details.empty()) {
        // Escapar caracteres especiales
        std::string escaped = details;
        size_t pos = 0;
        while ((pos = escaped.find('"', pos)) != std::string::npos) {
            escaped.replace(pos, 1, "\\\"");
            pos += 2;
        }
        oss << "\"details\":\"" << escaped << "\",";
    }
    
    // Contexto
    if (!userContext.empty()) {
        oss << "\"userContext\":\"" << userContext << "\",";
    }
    
    // Session ID
    if (!sessionId.empty()) {
        oss << "\"sessionId\":\"" << sessionId << "\",";
    }
    
    // Thread ID
    oss << "\"threadId\":" << std::hash<std::thread::id>{}(threadId);
    
    oss << "}";
    return oss.str();
}

// TraceFilter implementation
bool TraceFilter::matches(const TraceEntry& entry) const {
    // Nivel mínimo
    if (entry.level < minLevel) return false;
    
    // Tipos de evento
    if (!eventTypes.empty()) {
        bool found = false;
        for (const auto& type : eventTypes) {
            if (type == entry.eventType) {
                found = true;
                break;
            }
        }
        if (!found) return false;
    }
    
    // Componentes
    if (!components.empty()) {
        bool found = false;
        for (const auto& comp : components) {
            if (comp == entry.component) {
                found = true;
                break;
            }
        }
        if (!found) return false;
    }
    
    // Rango de tiempo
    if (entry.timestamp < startTime || entry.timestamp > endTime) return false;
    
    // Session ID
    if (!sessionId.empty() && entry.sessionId != sessionId) return false;
    
    return true;
}

// TraceLogger implementation
TraceLogger& TraceLogger::instance() {
    static TraceLogger instance;
    return instance;
}

TraceLogger::TraceLogger()
    : minLevel_(TraceLevel::DEBUG)
    , maxEntries_(10000)
    , consoleOutput_(true)
    , fileOutput_(false)
    , maxFileSize_(10 * 1024 * 1024) {
    entries_.reserve(1000);
    currentSessionId_ = generateSessionId();
}

TraceLogger::~TraceLogger() {
    flush();
    if (fileStream_.is_open()) {
        fileStream_.close();
    }
}

void TraceLogger::setLogFile(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (fileStream_.is_open()) {
        fileStream_.close();
    }
    
    logFilename_ = filename;
    fileStream_.open(filename, std::ios::out | std::ios::app);
    fileOutput_ = fileStream_.is_open();
}

void TraceLogger::setMinLevel(TraceLevel level) {
    minLevel_ = level;
}

void TraceLogger::setMaxEntries(size_t max) {
    maxEntries_ = max;
}

void TraceLogger::enableConsoleOutput(bool enable) {
    consoleOutput_ = enable;
}

void TraceLogger::enableFileOutput(bool enable) {
    fileOutput_ = enable;
}

void TraceLogger::log(TraceLevel level, TraceEventType type,
                    const std::string& component, const std::string& action,
                    const std::string& details,
                    const std::string& userContext) {
    // Verificar nivel mínimo
    if (level < minLevel_) return;
    
    TraceEntry entry(level, type, component, action, details, userContext);
    entry.sessionId = currentSessionId_;
    
    {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Rotar si es necesario
        if (entries_.size() >= maxEntries_) {
            entries_.erase(entries_.begin(), entries_.begin() + maxEntries_ / 4);
        }
        
        entries_.push_back(entry);
    }
    
    // Escribir a consola
    if (consoleOutput_) {
        writeToConsole(entry);
    }
    
    // Escribir a archivo
    if (fileOutput_ && fileStream_.is_open()) {
        writeToFile(entry);
        rotateLogIfNeeded();
    }
    
    // Notificar callbacks
    notifyCallbacks(entry);
}

void TraceLogger::debug(TraceEventType type, const std::string& comp, 
                      const std::string& action, const std::string& details) {
    log(TraceLevel::DEBUG, type, comp, action, details);
}

void TraceLogger::info(TraceEventType type, const std::string& comp,
                       const std::string& action, const std::string& details) {
    log(TraceLevel::INFO, type, comp, action, details);
}

void TraceLogger::warning(TraceEventType type, const std::string& comp,
                          const std::string& action, const std::string& details) {
    log(TraceLevel::WARNING, type, comp, action, details);
}

void TraceLogger::error(TraceEventType type, const std::string& comp,
                        const std::string& action, const std::string& details) {
    log(TraceLevel::ERR, type, comp, action, details);
}

void TraceLogger::critical(TraceEventType type, const std::string& comp,
                            const std::string& action, const std::string& details) {
    log(TraceLevel::CRITICAL, type, comp, action, details);
}

void TraceLogger::addCallback(TraceCallback callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    callbacks_.push_back(callback);
}

void TraceLogger::removeCallback(size_t index) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (index < callbacks_.size()) {
        callbacks_.erase(callbacks_.begin() + index);
    }
}

std::vector<TraceEntry> TraceLogger::query(const TraceFilter& filter) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<TraceEntry> result;
    
    for (const auto& entry : entries_) {
        if (filter.matches(entry)) {
            result.push_back(entry);
        }
    }
    
    return result;
}

std::vector<TraceEntry> TraceLogger::getRecent(size_t count) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<TraceEntry> result;
    
    size_t start = entries_.size() > count ? entries_.size() - count : 0;
    for (size_t i = start; i < entries_.size(); ++i) {
        result.push_back(entries_[i]);
    }
    
    return result;
}

std::vector<TraceEntry> TraceLogger::getByComponent(const std::string& component) {
    TraceFilter filter;
    filter.components = {component};
    return query(filter);
}

std::vector<TraceEntry> TraceLogger::getBySession(const std::string& sessionId) {
    TraceFilter filter;
    filter.sessionId = sessionId;
    return query(filter);
}

std::map<std::string, size_t> TraceLogger::getComponentStats() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::map<std::string, size_t> stats;
    
    for (const auto& entry : entries_) {
        stats[entry.component]++;
    }
    
    return stats;
}

std::map<TraceEventType, size_t> TraceLogger::getEventTypeStats() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::map<TraceEventType, size_t> stats;
    
    for (const auto& entry : entries_) {
        stats[entry.eventType]++;
    }
    
    return stats;
}

std::map<TraceLevel, size_t> TraceLogger::getLevelStats() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::map<TraceLevel, size_t> stats;
    
    for (const auto& entry : entries_) {
        stats[entry.level]++;
    }
    
    return stats;
}

void TraceLogger::exportToJson(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ofstream file(filename);
    
    if (!file.is_open()) return;
    
    file << "[" << std::endl;
    for (size_t i = 0; i < entries_.size(); ++i) {
        file << "  " << entries_[i].toJson();
        if (i < entries_.size() - 1) file << ",";
        file << std::endl;
    }
    file << "]" << std::endl;
}

void TraceLogger::exportToCsv(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ofstream file(filename);
    
    if (!file.is_open()) return;
    
    // Header
    file << "Timestamp,Level,EventType,Component,Action,Details,UserContext,SessionId" << std::endl;
    
    for (const auto& entry : entries_) {
        auto time_t = std::chrono::system_clock::to_time_t(entry.timestamp);
        auto tm = std::localtime(&time_t);
        char buf[30];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm);
        
        file << buf << ",";
        
        switch (entry.level) {
            case TraceLevel::DEBUG: file << "DEBUG"; break;
            case TraceLevel::INFO: file << "INFO"; break;
            case TraceLevel::WARNING: file << "WARNING"; break;
            case TraceLevel::ERR: file << "ERROR"; break;
            case TraceLevel::CRITICAL: file << "CRITICAL"; break;
        }
        file << ",";
        
        file << "\"" << entry.component << "\",\"" << entry.action << "\",";
        file << "\"" << entry.details << "\",\"" << entry.userContext << "\",";
        file << "\"" << entry.sessionId << "\"" << std::endl;
    }
}

void TraceLogger::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    entries_.clear();
}

void TraceLogger::flush() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (fileStream_.is_open()) {
        fileStream_.flush();
    }
}

std::string TraceLogger::generateSessionId() {
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    
    std::ostringstream oss;
    oss << "SESS-" << ms << "-" << std::rand();
    return oss.str();
}

std::string TraceLogger::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto tm = std::localtime(&time_t);
    char buf[30];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm);
    return std::string(buf);
}

void TraceLogger::writeToFile(const TraceEntry& entry) {
    if (!fileStream_.is_open()) return;
    
    fileStream_ << entry.toString() << std::endl;
}

void TraceLogger::writeToConsole(const TraceEntry& entry) {
    std::cout << entry.toString() << std::endl;
}

void TraceLogger::notifyCallbacks(const TraceEntry& entry) {
    for (const auto& callback : callbacks_) {
        callback(entry);
    }
}

void TraceLogger::rotateLogIfNeeded() {
    if (!fileStream_.is_open()) return;
    
    auto pos = fileStream_.tellp();
    if (pos > static_cast<std::streamoff>(maxFileSize_)) {
        // Cerrar y renombrar archivo
        fileStream_.close();
        
        std::string newName = logFilename_ + ".old";
        std::remove(newName.c_str());
        std::rename(logFilename_.c_str(), newName.c_str());
        
        // Reabrir
        fileStream_.open(logFilename_, std::ios::out | std::ios::app);
    }
}

// TraceScope implementation
TraceScope::TraceScope(const std::string& component, const std::string& action)
    : component_(component)
    , action_(action)
    , start_(std::chrono::high_resolution_clock::now()) {
    
    TraceLogger::instance().debug(TraceEventType::USER_ACTION, component_, action_ + ":BEGIN");
}

TraceScope::~TraceScope() {
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start_).count();
    
    std::ostringstream oss;
    oss << action_ << ":END (" << duration << " μs)";
    
    // Agregar detalles si existen
    if (!details_.empty()) {
        oss << " - ";
        for (const auto& [key, value] : details_) {
            oss << key << "=" << value << " ";
        }
    }
    
    TraceLogger::instance().debug(TraceEventType::USER_ACTION, component_, oss.str());
}

void TraceScope::addDetail(const std::string& key, const std::string& value) {
    details_[key] = value;
}

} // namespace tp::shared
