cmake_minimum_required (VERSION 2.8)

set (PROJECT rethinklog)
project(${PROJECT})

#
# protobuf
#
find_package(Protobuf)
include_directories(${PROTOBUF_INCLUDE_DIRS})
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS wire.proto)

#find_package(ZeroMQ)
#find_package(Boost)


add_executable(${PROJECT} main.cpp ${PROTO_SRCS})
target_link_libraries( ${PROJECT} ${PROTOBUF_LIBRARIES} )