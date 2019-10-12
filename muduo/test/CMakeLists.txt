ADD_EXECUTABLE(EventLoop_test1 EventLoop_test1.cpp)
TARGET_LINK_LIBRARIES(EventLoop_test1 muduo_net tools_log tools_base pthread)

ADD_EXECUTABLE(EventLoop_test2 EventLoop_test2.cpp)
TARGET_LINK_LIBRARIES(EventLoop_test2 muduo_net tools_log tools_base pthread)

ADD_EXECUTABLE(EventLoop_test3 EventLoop_test3.cpp)
TARGET_LINK_LIBRARIES(EventLoop_test3 muduo_net tools_log tools_base)

ADD_EXECUTABLE(timer_test timer_test.cpp)
TARGET_LINK_LIBRARIES(timer_test muduo_net tools_log tools_base)

ADD_EXECUTABLE(timer_test2 timer_test2.cpp)
TARGET_LINK_LIBRARIES(timer_test2 muduo_net tools_log tools_base)

ADD_EXECUTABLE(timer_test3 timer_test3.cpp)
TARGET_LINK_LIBRARIES(timer_test3 muduo_net tools_log tools_base pthread)

ADD_EXECUTABLE(Acceptor_test Acceptor_test.cpp)
TARGET_LINK_LIBRARIES(Acceptor_test muduo_net tools_socket tools_log tools_base)