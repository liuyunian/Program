CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
PROJECT(Muduo)

ADD_COMPILE_OPTIONS(-std=c++11)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})

# ======================== source =========================
SET(NET_SRCS
    Acceptor.cpp
    Channel.cpp
    EventLoop.cpp
    EventLoopThread.cpp
    Poller.cpp
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
    Channel.h
    EventLoop.h
    EventLoopThread.h
    TimerId.h)

INSTALL(FILES ${HEADERS} DESTINATION include/muduo)