#include <kernel/types.h>
#include <logger.h>
#include <kernel/drivers/serial.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

logger_level_t minimum_level = 0;
void logger_install(logger_level_t _minimum_level){
    serial_install(COM1);
    minimum_level = _minimum_level;
}
void log(logger_level_t level,string_t msg ,va_list arg){
    if(level>minimum_level){
        return;
    }
    char buf[512];
    vsprintf(buf,msg,arg);
    char buf2[512];
    char style[50] = "";

    switch (level)
    {
        case LOG_LEVEL_FATAL:
            strcpy(style,"\033[1;37;0;41m FATAL \033[m");
            break;
        case LOG_LEVEL_CRITICAL:
            strcpy(style,"\033[1;37;0;41m CRITICAL \033[m");
            break;
        case LOG_LEVEL_ERROR:
            strcpy(style,"\033[1;37;0;41m ERR \033[m");
            break;
        case LOG_LEVEL_WARNING:
            strcpy(style,"\033[1;37;0;43m WARN \033[m");
            break;
        case LOG_LEVEL_INFORMATION:
            strcpy(style,"\033[1;37;0;44m INFO \033[m");
            break;
        case LOG_LEVEL_DEBUG:
            strcpy(style," DEBUG ");
            break;
        case LOG_LEVEL_TRACE:
            strcpy(style," TRACE ");
            break;
        default:
            break;
    }
    sprintf(buf2,"\n[%s] %s",&style,&buf);
    serial_print(COM1,buf2);
    // printf(buf2);
}
void log_fatal(string_t str,...){
    va_list arg;
    va_start(arg,str);
    log(LOG_LEVEL_FATAL,str,arg);
}
void log_critical(string_t str,...){
    va_list arg;
    va_start(arg,str);
    log(LOG_LEVEL_CRITICAL,str,arg);
}
void log_error(string_t str,...){
    va_list arg;
    va_start(arg,str);
    log(LOG_LEVEL_ERROR,str,arg);
}
void log_warning(string_t str,...){
    va_list arg;
    va_start(arg,str);
    log(LOG_LEVEL_WARNING,str,arg);
}
void log_information(string_t str,...){
    va_list arg;
    va_start(arg,str);
    log(LOG_LEVEL_INFORMATION,str,arg);
}
void log_debug(string_t str,...){
    va_list arg;
    va_start(arg,str);
    log(LOG_LEVEL_DEBUG,str,arg);
}
void log_trace(string_t str,...){
    va_list arg;
    va_start(arg,str);
    log(LOG_LEVEL_TRACE,str,arg);
}