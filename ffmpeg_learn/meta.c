#include <libavutil/log.h>
#include <libavformat/avformat.h>

int main(){
    av_log_set_level(AV_LOG_INFO);

    // av_register_all();

    int ret;
    AVFormatContext * fmt_ctx = NULL;

    ret = avformat_open_input(&fmt_ctx, "/Users/lyn/out.mp4", NULL, NULL);
    if(ret < 0){
        av_log(NULL, AV_LOG_ERROR, "can't open the file: %s\n", av_err2str(ret));
        return -1;
    }

    av_dump_format(fmt_ctx, 0, "/Users/lyn/out.mp4", 0);

    avformat_close_input(&fmt_ctx);

    return 0;
}