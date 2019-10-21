# ======================== source =========================
SET(NET_SRCS
    Acceptor.cpp
    Channel.cpp
    EventLoop.cpp
    EventLoopThread.cpp
    Poller.cpp
    TcpConnection.cpp
    TcpServer.cpp
    Timer.cpp
    TimerQueue.cpp
    poller/PollPoller.cpp
    poller/EPollPoller.cpp)

# ========================= build ==========================
ADD_LIBRARY(muduo_net STATIC ${NET_SRCS})
TARGET_LINK_LIBRARIES(muduo_net tools_socket tools_log tools_base)

# ======================== install =========================
# libmuduo_net.a -- install path = /usr/local/lib
INSTALL(TARGETS muduo_net ARCHIVE DESTINATION lib)

# *.h -- install path = /usr/local/include/muduo
set(HEADERS
    Acceptor.h
    Callbacks.h
    Channel.h
    EventLoop.h
    EventLoopThread.h
    TcpConnection.h
    TcpServer.h
    TimerId.h)

INSTALL(FILES ${HEADERS} DESTINATION include/muduo)