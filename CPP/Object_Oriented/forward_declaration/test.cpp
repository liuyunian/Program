#include "EventLoop.h"
#include "Channel.h"

int main(){
    EventLoop loop;
    Channel ch(&loop);

    return 0;
}