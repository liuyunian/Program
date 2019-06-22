#include "log.h"

int main(){
    log_set_level(LOG_INFO);
    log_err(ERR_NSYS, "hello world %d", 10);
    log_err(ERR_SYS, "hello world %d", 10);
    // log_fatal(FAT_NSYS, "hello world %d", 10);
    // log_fatal(FAT_SYS, "hello world %d", 10);
    // log_fatal(FAT_DUMP, "hello world %d", 10);

    log_warn("hello world %d", 10);
    log_info("hello world %d", 10);
    log_debug("hello world %d", 10);
    return 0;
}