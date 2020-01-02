# ======================== source =========================
SET(MUDUO_SRC
  Acceptor.cpp
  Channel.cpp
  EventLoop.cpp
  EventLoopThread.cpp
  EventLoopThreadPool.cpp
  Poller.cpp
  TCPConnection.cpp
  TCPServer.cpp
  Timer.cpp
  TimerQueue.cpp
  poller/PollPoller.cpp
  poller/EPollPoller.cpp
)

set(MUDUO_HDR
  Buffer.h
  Callbacks.h
  EventLoop.h
  TCPConnection.h
  TCPServer.h
  TimerId.h
)

# ========================= build ==========================
ADD_LIBRARY(muduo_net STATIC ${MUDUO_SRC})
TARGET_LINK_LIBRARIES(muduo_net tools_socket tools_log tools_base)

# ======================== install =========================
# libmuduo_net.a -- install path = /usr/local/lib
INSTALL(TARGETS muduo_net ARCHIVE DESTINATION lib)

# *.h -- install path = /usr/local/include/muduo
INSTALL(FILES ${MUDUO_HDR} DESTINATION include/muduo)