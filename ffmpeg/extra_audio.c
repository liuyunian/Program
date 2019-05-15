#include <stdio.h>
#include <libavutil/log.h>
#include <libavformat/avformat.h>

int main(int argc, char * argv[]){
    av_log_set_level(AV_LOG_INFO);

    int ret;
    FILE * audio_fd = NULL;
    AVFormatContext * fmt_ctx = NULL;

    // 1、读取参数列表，获得媒体数据的源路径和抽取音频数据的目的路径
    if(argc < 3){
        av_log(NULL, AV_LOG_ERROR, "invalid list of param\n");
        return -1;
    }
    char * srcPath = argv[1];
    char * distPath = argv[2];

    // 2、打开源媒体文件，获得格式上下文fmt_ctx
    ret = avformat_open_input(&fmt_ctx, srcPath, NULL, NULL);
    if(ret < 0){
        av_log(NULL, AV_LOG_ERROR, "can't open the file: %s\n", av_err2str(ret));
        return -1;
    }

    // 3、输出媒体信息
    av_dump_format(fmt_ctx, 0, srcPath, 0);

    // 4、获取音频流
    int audio_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if(audio_index < 0){
        av_log(NULL, AV_LOG_ERROR, "can't find the audio stream\n");
        goto __fail;
    }

    av_log(NULL, AV_LOG_INFO, "get audio stream\n");

    // 5、创建或打开保存音频的文件
    audio_fd = fopen(distPath, "w");
    if(!audio_fd){
        av_log(NULL, AV_LOG_ERROR, "can't open the audio file\n");
        goto __fail;
    }
    av_log(NULL, AV_LOG_INFO, "open the audio file\n");

    // 6、获取流中的数据
    AVPacket pkt;
    av_init_packet(&pkt);
    while(av_read_frame(fmt_ctx, &pkt) >= 0){
        if(pkt.stream_index == audio_index){
            int len = fwrite(pkt.data, 1, pkt.size, audio_fd);
            if(len != pkt.size){
                av_log(NULL, AV_LOG_ERROR, "fail to write audio data\n");
                goto __fail;
            }
        }

        av_packet_unref(&pkt);
    }

__fail:
    if(!audio_fd){
        fclose(audio_fd);
    }

    avformat_close_input(&fmt_ctx);

    return 0;
}