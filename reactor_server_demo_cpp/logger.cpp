// logger.cpp - 线程安全日志系统实现
#include "logger.h"

#include <sys/syscall.h>
#include <sys/time.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>

Logger& Logger::instance() {
    static Logger logger;  // 单例
    return logger;
}

const char* Logger::levelName(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO:  return "INFO";
        case LogLevel::WARN:  return "WARN";
        case LogLevel::ERROR: return "ERROR";
    }
    return "UNKNOWN";
}

void Logger::log(LogLevel level, const char* file, int line, const char* fmt, ...) {
    if (level < level_) return;

    // 时间戳（精确到毫秒）
    timeval tv{};
    ::gettimeofday(&tv, nullptr);
    tm tmv{};
    ::localtime_r(&tv.tv_sec, &tmv);
    char time_buf[64];
    ::snprintf(time_buf, sizeof(time_buf), "%04d-%02d-%02d %02d:%02d:%02d.%03d",
               tmv.tm_year + 1900, tmv.tm_mon + 1, tmv.tm_mday,
               tmv.tm_hour, tmv.tm_min, tmv.tm_sec,
               static_cast<int>(tv.tv_usec / 1000));

    // 线程 ID
    pid_t tid = ::syscall(SYS_gettid);

    // 格式化正文
    char body[1024];
    va_list args;
    va_start(args, fmt);
    ::vsnprintf(body, sizeof(body), fmt, args);
    va_end(args);

    // 只保留文件名
    const char* base = ::strrchr(file, '/');
    base = base ? base + 1 : file;

    std::lock_guard<std::mutex> lock(mtx_);
    ::fprintf(stdout, "[%s][%s][tid %d][%s:%d] %s\n",
              time_buf, levelName(level), tid, base, line, body);
    ::fflush(stdout);
}
