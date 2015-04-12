#ifndef LOG_H

#include <cstdio>
#include <string>

enum LogLevel {
    DEBUG,
    WARNING,
    ERROR
};

void log_init(FILE*);
void log_init(std::string);
void log(LogLevel lvl, std::string message);
void log_free();

#endif
