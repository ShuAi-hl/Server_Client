#pragma once
// logger.h - 线程安全日志系统：级别过滤、时间戳、线程 ID
#include <cstdarg>
#include <mutex>
#include <string>

enum class LogLevel {
    DEBUG = 0,
    INFO,
    WARN,
    ERROR,
};

class Logger {
public:
    static Logger& instance();

    void setLevel(LogLevel level) { level_ = level; }
    LogLevel level() const { return level_; }

    // 格式化输出一条日志（stdout，带时间戳与线程 ID）
    void log(LogLevel level, const char* file, int line, const char* fmt, ...)
        __attribute__((format(printf, 5, 6)));

private:
    Logger() = default;
    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    static const char* levelName(LogLevel level);

    std::mutex mtx_;
    LogLevel level_ = LogLevel::INFO;
};

#define LOG_DEBUG(...) Logger::instance().log(LogLevel::DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...)  Logger::instance().log(LogLevel::INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARN(...)  Logger::instance().log(LogLevel::WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) Logger::instance().log(LogLevel::ERROR, __FILE__, __LINE__, __VA_ARGS__)
