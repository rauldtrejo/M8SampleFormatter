#include "Logger.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::setLevel(Level level) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_level = level;
}

void Logger::setLogFile(const std::string& filename) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_logFile = filename;
    m_fileStream = std::make_unique<std::ofstream>(filename, std::ios::app);
}

void Logger::debug(const std::string& message) {
    log(DEBUG, message);
}

void Logger::info(const std::string& message) {
    log(INFO, message);
}

void Logger::warning(const std::string& message) {
    log(WARNING, message);
}

void Logger::error(const std::string& message) {
    log(ERROR, message);
}

void Logger::log(Level level, const std::string& message) {
    if (level < m_level) return;
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::stringstream ss;
    ss << getTimestamp() << " [" << levelToString(level) << "] " << message << std::endl;
    
    // Output to console
    if (level >= WARNING) {
        std::cerr << ss.str();
    } else {
        std::cout << ss.str();
    }
    
    // Output to file if set
    if (m_fileStream && m_fileStream->is_open()) {
        *m_fileStream << ss.str();
        m_fileStream->flush();
    }
}

std::string Logger::levelToString(Level level) {
    switch (level) {
        case DEBUG: return "DEBUG";
        case INFO: return "INFO";
        case WARNING: return "WARN";
        case ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

std::string Logger::getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}
