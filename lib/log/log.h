#ifndef LOG_H
#define LOG_H

#include <stdarg.h> // va_list va_start va_end

enum Log_level{
    LOG_FATAL = 16,
    LOG_ERR = 8,
    LOG_WARN = 4,
    LOG_INFO = 2,
    LOG_DEBUG = 1
};

enum Error_type{
    ERR_NSYS,
    ERR_SYS
};

enum Fatal_type{
    FAT_NSYS,
    FAT_SYS,
    FAT_DUMP
};

void log_set_level(int level);

void log_err(int err_level, const char * fmt, ...);

void log_fatal(int fat_level, const char * fmt, ...);

void log_warn(const char * fmt, ...);

void log_info(const char * fmt, ...);

void log_debug(const char * fmt, ...);

#endif