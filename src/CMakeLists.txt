cmake_minimum_required (VERSION 2.8)

set (PROJECT rethinklog)
project(${PROJECT})

#
# libraries
#

#	protobuf
#
find_package(Protobuf REQUIRED)  
include_directories(${PROTOBUF_INCLUDE_DIRS})
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS wire.proto)

#	ZeroMQ
#

#	Boost
#
SET(Boost_USE_STATIC_LIBS ON)
find_package( Boost COMPONENTS log REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})


#
# own sources
# 
file(GLOB sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h)


#
# build
#

#	server
add_executable(	${PROJECT} 
				${sources} 
				${headers}
				wire.proto ${PROTO_SRCS} ${PROTO_HDRS} )

target_link_libraries(	${PROJECT} 
						${PROTOBUF_LIBRARIES}
						${Boost_log_LIBRARY} )

#	client library


#	tests
