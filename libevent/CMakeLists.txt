cmake_minimum_required(VERSION 3.10)
project(Test_libevent)
add_executable(Test_libevent test_libevent.cpp)
target_link_libraries(Test_libevent event)