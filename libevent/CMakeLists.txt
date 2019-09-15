CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
PROJECT(Test_libevent)
ADD_EXECUTABLE(test_libevent test_libevent.cpp)
TARGET_LINK_LIBRARIES(test_libevent event)