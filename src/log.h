#ifndef LOG_H
#define LOG_H

#include <cstdio>
#include <string>

enum LogLevel {
    LOG_DEBUG,
    LOG_WARNING,
    LOG_ERROR
};

void log_init(FILE*);
void log_init(std::string);
void log(LogLevel lvl, std::string message);
void log_free();

#endif
