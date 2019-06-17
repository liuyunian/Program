#include <libavformat/avformat.h>

int main(){
    int ret = avpriv_io_move("./test.txt", "../test.txt");
    if(ret < 0){
        av_log(NULL, AV_LOG_ERROR, "fail to move file test.txt\n");
        return -1;
    }
    av_log(NULL, AV_LOG_INFO, "finish to move file test.txt\n");

    return 0;
}