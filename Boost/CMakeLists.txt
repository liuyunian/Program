CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(Boost_test)

ADD_COMPILE_OPTIONS(-std=c++14)
INCLUDE_DIRECTORIES(/usr/local/include)
LINK_DIRECTORIES(/usr/local/lib)

ADD_EXECUTABLE(asio_signal asio_signal.cpp)

ADD_EXECUTABLE(asio_timer asio_timer.cpp)

ADD_EXECUTABLE(ptree_info ptree_info.cpp)

ADD_EXECUTABLE(filesystem_path filesystem_path.cpp)
TARGET_LINK_LIBRARIES(filesystem_path boost_filesystem)