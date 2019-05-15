#include <libavutil/log.h>
#include <libavformat/avformat.h>

#ifndef AV_WB32
#define AV_WB32(p, val) do {                 \
        uint32_t d = (val);                  \
        ((uint8_t*)(p))[3] = (d);            \
        ((uint8_t*)(p))[2] = (d)>>8;         \
        ((uint8_t*)(p))[1] = (d)>>16;        \
        ((uint8_t*)(p))[0] = (d)>>24;        \
    } while(0)
#endif

#ifndef AV_RB16
#define AV_RB16(x)                           \
    ((((const uint8_t*)(x))[0] << 8) |       \
      ((const uint8_t*)(x))[1])
#endif

static int alloc_and_copy(AVPacket *out, const uint8_t *sps_pps, uint32_t sps_pps_size, const uint8_t *in, uint32_t in_size){
    uint32_t offset         = out->size;
    uint8_t nal_header_size = offset ? 3 : 4;
    int err;

    err = av_grow_packet(out, sps_pps_size + in_size + nal_header_size);
    if (err < 0)
        return err;

    if (sps_pps)
        memcpy(out->data + offset, sps_pps, sps_pps_size);
    memcpy(out->data + sps_pps_size + nal_header_size + offset, in, in_size);
    if (!offset) {
        AV_WB32(out->data + sps_pps_size, 1);
    } else {
        (out->data + offset + sps_pps_size)[0] =
        (out->data + offset + sps_pps_size)[1] = 0;
        (out->data + offset + sps_pps_size)[2] = 1;
    }

    return 0;
}

int h264_extradata_to_annexb(const uint8_t *codec_extradata, const int codec_extradata_size, AVPacket *out_extradata, int padding){
    uint16_t unit_size;
    uint64_t total_size                 = 0;
    uint8_t *out                        = NULL, unit_nb, sps_done = 0,
             sps_seen                   = 0, pps_seen = 0, sps_offset = 0, pps_offset = 0;
    const uint8_t *extradata            = codec_extradata + 4;
    static const uint8_t nalu_header[4] = { 0, 0, 0, 1 };
    int length_size = (*extradata++ & 0x3) + 1; // retrieve length coded size, 用于指示表示编码数据长度所需字节数

    sps_offset = pps_offset = -1;

    /* retrieve sps and pps unit(s) */
    unit_nb = *extradata++ & 0x1f; /* number of sps unit(s) */
    if (!unit_nb) {
        goto pps;
    }else {
        sps_offset = 0;
        sps_seen = 1;
    }

    while (unit_nb--) {
        int err;

        unit_size   = AV_RB16(extradata);
        total_size += unit_size + 4;
        if (total_size > INT_MAX - padding) {
            av_log(NULL, AV_LOG_ERROR,
                   "Too big extradata size, corrupted stream or invalid MP4/AVCC bitstream\n");
            av_free(out);
            return AVERROR(EINVAL);
        }
        if (extradata + 2 + unit_size > codec_extradata + codec_extradata_size) {
            av_log(NULL, AV_LOG_ERROR, "Packet header is not contained in global extradata, "
                   "corrupted stream or invalid MP4/AVCC bitstream\n");
            av_free(out);
            return AVERROR(EINVAL);
        }
        if ((err = av_reallocp(&out, total_size + padding)) < 0)
            return err;
        memcpy(out + total_size - unit_size - 4, nalu_header, 4);
        memcpy(out + total_size - unit_size, extradata + 2, unit_size);
        extradata += 2 + unit_size;
pps:
        if (!unit_nb && !sps_done++) {
            unit_nb = *extradata++; /* number of pps unit(s) */
            if (unit_nb) {
                pps_offset = total_size;
                pps_seen = 1;
            }
        }
    }

    if (out)
        memset(out + total_size, 0, padding);

    if (!sps_seen)
        av_log(NULL, AV_LOG_WARNING,
               "Warning: SPS NALU missing or invalid. "
               "The resulting stream may not play.\n");

    if (!pps_seen)
        av_log(NULL, AV_LOG_WARNING,
               "Warning: PPS NALU missing or invalid. "
               "The resulting stream may not play.\n");

    out_extradata->data      = out;
    out_extradata->size      = total_size;

    return length_size;
}

void h264_mp4toannexb(AVFormatContext * fmt_ctx, AVPacket * in, FILE * video_fd){
    AVPacket *out = NULL;
    AVPacket spspps_pkt;
    out = av_packet_alloc();

    const uint8_t * buf = in->data;
    const uint8_t * buf_end = in->data + in->size;
    int buf_size = in->size;
    uint32_t cumul_size = 0;

/*
    在MP4文件中存储的h264数据并不是以startCode + NALU格式存储的，而是NALU的length（4字节）+ NALU的header（1字节）+ payload
*/
    do{
        if(buf+4 > buf_end){
            av_log(NULL, AV_LOG_ERROR, "packet is invalid: length < 4\n");
            goto fail;
        }

        int32_t nal_size = 0;
        for(int i = 0; i < 4; ++ i){
            nal_size = (nal_size << 8) | buf[i];
        }
        buf += 4; //偏移4字节
        if(nal_size > buf_end - buf || nal_size < 0){
            av_log(NULL, AV_LOG_ERROR, "packet is invalid: nal_size is illegal\n");
            goto fail;
        }
            
        uint8_t unit_type = *buf & 0x1f; // 获取NALU header中的NALU类型值(一个字节中的后5位)

        if(unit_type == 5){ // 类型值为5：关键帧（I帧）
            h264_extradata_to_annexb(fmt_ctx->streams[in->stream_index]->codec->extradata,
                                      fmt_ctx->streams[in->stream_index]->codec->extradata_size,
                                      &spspps_pkt,
                                      AV_INPUT_BUFFER_PADDING_SIZE);

            int ret = alloc_and_copy(out, spspps_pkt.data, spspps_pkt.size, buf, nal_size);
            if (ret < 0){
                av_log(NULL, AV_LOG_ERROR, "fail to add startCode for I frame \n");
                goto fail;
            }
        }
        else {
            int ret = alloc_and_copy(out, NULL, 0, buf, nal_size);
            if(ret < 0){
                av_log(NULL, AV_LOG_ERROR, "fail to add startCode for other frame \n");
                goto fail;
            }    
        }

        int len = fwrite(out->data, 1, out->size, video_fd);
        if(len != out->size){
            av_log(NULL, AV_LOG_ERROR, "warning, length of writed data isn't equal pkt.size(%d, %d)\n", len, out->size);
        }
        fflush(video_fd);

        buf += nal_size;
        cumul_size += nal_size + 4;//s->length_size;
    }
    while (cumul_size < buf_size); // 一个packet中可能有多个帧
    
fail:
    av_packet_free(&out);
}

int main(int argc, char * argv[]){
    av_log_set_level(AV_LOG_INFO);

    if(argc < 3){
        av_log(NULL, AV_LOG_ERROR, "invalid list of param\n");
        return 0;
    }
    char * srcPath = argv[1];
    char * distPath = argv[2];

    AVFormatContext * fmt_ctx = NULL;
    int ret = avformat_open_input(&fmt_ctx, srcPath, NULL, NULL);
    if(ret < 0){
        av_log(NULL, AV_LOG_ERROR, "can't open the source file: %s\n", av_err2str(ret));
        return -1;
    }

    av_dump_format(fmt_ctx, 0, srcPath, 0);

    int video_stream_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if(video_stream_index < 0){
        av_log(NULL, AV_LOG_ERROR, "can't find the audio stream\n");
        goto __fail;
    }

    FILE * video_fd = fopen(distPath, "w");
    if(!video_fd){
        av_log(NULL, AV_LOG_ERROR, "can't open the audio file\n");
        goto __fail;
    }

    AVPacket pkt;
    av_init_packet(&pkt);
    while(av_read_frame(fmt_ctx, &pkt) >=0 ){
        if(pkt.stream_index == video_stream_index){
            h264_mp4toannexb(fmt_ctx, &pkt, video_fd);
        }

        av_packet_unref(&pkt);
    }

__fail:
    if(!video_fd){
        fclose(video_fd);
    }

    avformat_close_input(&fmt_ctx);

    return 0;
}