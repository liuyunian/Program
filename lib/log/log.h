#ifndef LOG_H
#define LOG_H

#include <stdarg.h> // va_list va_start va_end

/**
 * log level
 */
#define LOG_FATAL 16
#define LOG_ERR 8
#define LOG_WARN 4
#define LOG_INFO 2
#define LOG_DEBUG 1

/**
 * error level
 */
#define ERR_NSYS 0
#define ERR_SYS 1

/**
 * fatal level
 */
#define FAT_DUMP 2
#define FAT_SYS 1
#define FAT_NSYS 0

void log_set_level(int level);

void log_err(int err_level, const char * fmt, ...);

void log_fatal(int fat_level, const char * fmt, ...);

void log_warn(const char * fmt, ...);

void log_info(const char * fmt, ...);

void log_debug(const char * fmt, ...);

static void 
err_doit(int log_level, int errno_flag, const char * fmt, va_list ap);

static void 
info_doit(int log_level, const char * fmt, va_list ap);

#endif