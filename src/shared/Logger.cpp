#include "Logger.hpp"
#include <iostream>

namespace tp::shared {

Logger& Logger::instance() {
    static Logger instance;
    return instance;
}

Logger::Logger()
    : currentLevel_(LogLevel::LOG_INFO)
    , consoleOutput_(true) {}

Logger::~Logger() {
    if (logFile_.is_open()) {
        logFile_.close();
    }
}

void Logger::setLogLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(mutex_);
    currentLevel_ = level;
}

void Logger::setOutputFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (logFile_.is_open()) {
        logFile_.close();
    }
    logFile_.open(filepath, std::ios::app);
}

void Logger::enableConsoleOutput(bool enable) {
    std::lock_guard<std::mutex> lock(mutex_);
    consoleOutput_ = enable;
}

std::string Logger::getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::TRACE: return "TRACE";
        case LogLevel::LOG_DEBUG: return "DEBUG";
        case LogLevel::LOG_INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::LOG_ERROR: return "ERROR";
        case LogLevel::FATAL: return "FATAL";
        default: return "UNKNOWN";
    }
}

void Logger::writeLog(const std::string& logMessage) {
    if (consoleOutput_) {
        std::cout << logMessage << std::endl;
    }
    if (logFile_.is_open()) {
        logFile_ << logMessage << std::endl;
        logFile_.flush();
    }
}

void Logger::log(LogLevel level, const std::string& message, const std::string& file, int line) {
    if (level < currentLevel_) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::stringstream ss;
    ss << "[" << getTimestamp() << "] ";
    ss << "[" << levelToString(level) << "] ";
    
    if (!file.empty()) {
        size_t pos = file.find_last_of("/\\");
        std::string filename = (pos != std::string::npos) ? file.substr(pos + 1) : file;
        ss << "[" << filename << ":" << line << "] ";
    }
    
    ss << message;
    
    writeLog(ss.str());
}

void Logger::trace(const std::string& message, const std::string& file, int line) {
    log(LogLevel::TRACE, message, file, line);
}

void Logger::debug(const std::string& message, const std::string& file, int line) {
    log(LogLevel::LOG_DEBUG, message, file, line);
}

void Logger::info(const std::string& message, const std::string& file, int line) {
    log(LogLevel::LOG_INFO, message, file, line);
}

void Logger::warning(const std::string& message, const std::string& file, int line) {
    log(LogLevel::WARNING, message, file, line);
}

void Logger::error(const std::string& message, const std::string& file, int line) {
    log(LogLevel::LOG_ERROR, message, file, line);
}

void Logger::fatal(const std::string& message, const std::string& file, int line) {
    log(LogLevel::FATAL, message, file, line);
}

}
