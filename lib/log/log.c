#include <stdarg.h> // va_list va_start va_end
#include <errno.h> // errno
#include <stdio.h> //stdout stderr
#include <string.h> // strlen strcat
#include <stdlib.h> // exit abort

#include "log.h"

#define LINE_SZ 4096

static int show_level = 0;

void log_set_level(int level){
    show_level = level >> 1;
}

void log_err(int err_level, const char * fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    err_doit(LOG_ERR, err_level, fmt, ap);
    va_end(ap);

    return;
}

void log_fatal(int fat_level, const char * fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    err_doit(LOG_FATAL, fat_level, fmt, ap);
    va_end(ap);

    if(fat_level >> 1){
        abort();
    }
    
    exit(1);
}

void log_warn(const char * fmt, ...){
    if(!(LOG_WARN >> show_level)){
        return;
    }

    va_list ap;
    va_start(ap, fmt);
    info_doit(LOG_WARN, fmt, ap);
    va_end(ap);
}

void log_info(const char * fmt, ...){
    if(!(LOG_INFO >> show_level)){
        return;
    }
    va_list ap;
    va_start(ap, fmt);
    info_doit(LOG_INFO, fmt, ap);
    va_end(ap);
}

void log_debug(const char * fmt, ...){
    if(!(LOG_DEBUG >> show_level)){
        return;
    }

    va_list ap;
    va_start(ap, fmt);
    info_doit(LOG_DEBUG, fmt, ap);
    va_end(ap);
}

static void 
err_doit(int log_level, int errno_flag, const char * fmt, va_list ap){
    int errno_save, buf_len;
    char buf[LINE_SZ + 1] = {0};

    errno_save = errno;

    switch (log_level){
        case LOG_ERR:
            strcat(buf, "[ERROR] ");
            break;
        case LOG_FATAL:
            strcat(buf, "[FATAL] ");
            break;
        default:
            break;
    }

    buf_len = strlen(buf);

#ifdef HAVE_VSNPRINTF
    vsnprintf(buf + buf_len, LINE_SZ - buf_len, fmt, ap);
#else
    vsprintf(buf + buf_len, fmt, ap);
#endif

    buf_len = strlen(buf);
    if(errno_flag){
        snprintf(buf + buf_len, LINE_SZ - buf_len, ": %s", strerror(errno_save));
    }
    strcat(buf, "\n");

    fflush(stdout);
    fputs(buf, stderr);
    fflush(stderr);

    return;
}

static void
info_doit(int log_level, const char * fmt, va_list ap){
    int buf_len;
    char buf[LINE_SZ + 1] = {0};

    switch (log_level){
        case LOG_WARN:
            strcat(buf, "[WARNING] ");
            break;
        case LOG_INFO:
            strcat(buf, "[INFO] ");
            break;
        case LOG_DEBUG:
            strcat(buf, "[DEBUG] ");
        default:
            break;
    }

    buf_len = strlen(buf);

#ifdef HAVE_VSNPRINTF
    vsnprintf(buf + buf_len, LINE_SZ - buf_len, fmt, ap);
#else
    vsprintf(buf + buf_len, fmt, ap);
#endif
    strcat(buf, "\n");

    fflush(stdout);
    fputs(buf, stdout);
    fflush(stdout);

    return;
}
