CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
PROJECT(Muduo)

# ======================== source =========================
SET(NET_SRCS 
    Channel.cpp
    EventLoop.cpp
    Poller.cpp
    poller/PollPoller.cpp
    poller/EPollPoller.cpp)

# ========================= build ==========================
ADD_LIBRARY(muduo_net STATIC ${NET_SRCS})
TARGET_LINK_LIBRARIES(muduo_net tools_log tools_base)

# ======================== install =========================
# libmuduo_net.a -- install path = /usr/local/lib
INSTALL(TARGETS muduo_net ARCHIVE DESTINATION lib)

# *.h -- install path = /usr/local/include/muduo/net
set(HEADERS
    Channel.h
    EventLoop.h)

INSTALL(FILES ${HEADERS} DESTINATION include/muduo/net)

# ======================== test =============================
ADD_SUBDIRECTORY(test)