#include <stdio.h>
#include <libavutil/log.h>

int main(){
    av_log_set_level(AV_LOG_DEBUG);

    av_log(NULL, AV_LOG_INFO, "hello world\n");

    av_log(NULL, AV_LOG_INFO, "hello %s\n", "world");

    int a = 10;

    av_log(NULL, AV_LOG_INFO, "hello %d\n", a);

    return 0;
}