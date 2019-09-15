CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
PROJECT(Muduo)

ADD_EXECUTABLE(EventLoop_test1 EventLoop_test1.cpp EventLoop.cpp)
TARGET_LINK_LIBRARIES(EventLoop_test1 tools_log tools_base pthread)

ADD_EXECUTABLE(EventLoop_test2 EventLoop_test2.cpp EventLoop.cpp)
TARGET_LINK_LIBRARIES(EventLoop_test2 tools_log tools_base pthread)