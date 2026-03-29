#pragma once

/**
 * @file Logger.hpp
 * @brief Sistema de logging centralizado
 * @author Proyecto Taller Paramétrico
 * @version 0.2.3
 * 
 * Proporciona logging thread-safe con niveles:
 * - TRACE: Información de debug más detallada
 * - DEBUG: Mensajes de depuración
 * - INFO: Información general
 * - WARNING: Advertencias
 * - ERROR: Errores
 * - FATAL: Errores críticos
 * 
 * @note Thread-safe con std::mutex
 * @see TraceLogger para logging avanzado
 */

#include <string>
#include <fstream>
#include <sstream>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <iostream>

namespace tp::shared {

enum class LogLevel {
    TRACE,
    LOG_DEBUG,
    LOG_INFO,
    WARNING,
    LOG_ERROR,
    FATAL
};

class Logger {
public:
    static Logger& instance();
    
    void setLogLevel(LogLevel level);
    void setOutputFile(const std::string& filepath);
    void enableConsoleOutput(bool enable);
    
    void log(LogLevel level, const std::string& message, const std::string& file = "", int line = 0);
    void trace(const std::string& message, const std::string& file = "", int line = 0);
    void debug(const std::string& message, const std::string& file = "", int line = 0);
    void info(const std::string& message, const std::string& file = "", int line = 0);
    void warning(const std::string& message, const std::string& file = "", int line = 0);
    void error(const std::string& message, const std::string& file = "", int line = 0);
    void fatal(const std::string& message, const std::string& file = "", int line = 0);
    
private:
    Logger();
    ~Logger();
    
    std::string getTimestamp();
    std::string levelToString(LogLevel level);
    void writeLog(const std::string& logMessage);
    
    LogLevel currentLevel_;
    std::ofstream logFile_;
    bool consoleOutput_;
    std::mutex mutex_;
};

#define LOG_TRACE(msg) tp::shared::Logger::instance().trace(msg, __FILE__, __LINE__)
#define LOG_DEBUG_MACRO(msg) tp::shared::Logger::instance().debug(msg, __FILE__, __LINE__)
#define LOG_INFO_MACRO(msg) tp::shared::Logger::instance().info(msg, __FILE__, __LINE__)
#define LOG_WARNING(msg) tp::shared::Logger::instance().warning(msg, __FILE__, __LINE__)
#define LOG_ERROR_MACRO(msg) tp::shared::Logger::instance().error(msg, __FILE__, __LINE__)
#define LOG_FATAL(msg) tp::shared::Logger::instance().fatal(msg, __FILE__, __LINE__)

}
