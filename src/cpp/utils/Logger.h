#pragma once

#include <string>
#include <fstream>
#include <mutex>
#include <memory>

class Logger {
public:
    enum Level {
        DEBUG = 0,
        INFO = 1,
        WARNING = 2,
        ERROR = 3
    };

    static Logger& getInstance();
    
    void setLevel(Level level);
    void setLogFile(const std::string& filename);
    
    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
    
    void log(Level level, const std::string& message);

private:
    Logger() = default;
    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    Level m_level = INFO;
    std::string m_logFile;
    std::mutex m_mutex;
    std::unique_ptr<std::ofstream> m_fileStream;
    
    std::string levelToString(Level level);
    std::string getTimestamp();
};
