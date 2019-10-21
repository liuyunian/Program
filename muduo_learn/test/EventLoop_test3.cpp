#include <iostream>

#include <unistd.h>             // close
#include <string.h>             // memset
#include <sys/timerfd.h>        // timerfd_create timerfd_settime
#include <tools/base/Timestamp.h>
#include <muduo/EventLoop.h>
#include <muduo/Channel.h>

EventLoop * g_loop;
int timerfd;

void timeout(Timestamp recvTime){
    std::cout << "Timeout: " << recvTime.to_formatted_string() << std::endl;

    uint64_t howmany;
	read(timerfd, &howmany, sizeof howmany);
	g_loop->quit();
}

int main(){
    EventLoop loop;
    g_loop = &loop;

    timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    Channel channel(&loop, timerfd);
    channel.set_read_callback(timeout);
    channel.enable_reading();

    struct itimerspec howlong;
	memset(&howlong, 0, sizeof howlong);
	howlong.it_value.tv_sec = 1;
	timerfd_settime(timerfd, 0, &howlong, NULL);

    loop.loop();
    close(timerfd);

    return 0;
}

