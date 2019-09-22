CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(Boost_test)

ADD_COMPILE_OPTIONS(-std=c++14)
INCLUDE_DIRECTORIES(/usr/local/include)
LINK_DIRECTORIES(/usr/local/lib)

ADD_EXECUTABLE(asio_signal asio_signal.cpp)

ADD_EXECUTABLE(asio_timer asio_timer.cpp)
# TARGET_LINK_LIBRARIES(asio_signal boost_system boost_thread)