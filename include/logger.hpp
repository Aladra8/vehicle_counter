#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <iomanip>

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }
    
    void setLogFile(const std::string& filename) {
        log_file_.open(filename, std::ios::app);
    }
    
    template<typename... Args>
    void log(LogLevel level, Args&&... args) {
        std::string level_str = getLevelString(level);
        std::string timestamp = getCurrentTimestamp();
        
        std::stringstream ss;
        ss << "[" << timestamp << "] [" << level_str << "] ";
        (ss << ... << args);
        
        // Console output
        if (level >= LogLevel::WARNING) {
            std::cerr << ss.str() << std::endl;
        } else {
            std::cout << ss.str() << std::endl;
        }
        
        // File output
        if (log_file_.is_open()) {
            log_file_ << ss.str() << std::endl;
        }
    }
    
    template<typename... Args>
    void debug(Args&&... args) { log(LogLevel::DEBUG, std::forward<Args>(args)...); }
    
    template<typename... Args>
    void info(Args&&... args) { log(LogLevel::INFO, std::forward<Args>(args)...); }
    
    template<typename... Args>
    void warning(Args&&... args) { log(LogLevel::WARNING, std::forward<Args>(args)...); }
    
    template<typename... Args>
    void error(Args&&... args) { log(LogLevel::ERROR, std::forward<Args>(args)...); }
    
private:
    Logger() = default;
    std::ofstream log_file_;
    
    std::string getLevelString(LogLevel level) {
        switch (level) {
            case LogLevel::DEBUG: return "DEBUG";
            case LogLevel::INFO: return "INFO";
            case LogLevel::WARNING: return "WARNING";
            case LogLevel::ERROR: return "ERROR";
            default: return "UNKNOWN";
        }
    }
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }
};

#define LOG_DEBUG(...) Logger::getInstance().debug(__VA_ARGS__)
#define LOG_INFO(...) Logger::getInstance().info(__VA_ARGS__)
#define LOG_WARNING(...) Logger::getInstance().warning(__VA_ARGS__)
#define LOG_ERROR(...) Logger::getInstance().error(__VA_ARGS__)

#endif // LOGGER_HPP
