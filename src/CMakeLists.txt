cmake_minimum_required (VERSION 2.8)

set (PROJECT rethinklog)
project(${PROJECT})


find_package(protobuf)
find_package(zeromq)
find_package(Boost)

add_executable(${PROJECT} main.cpp)