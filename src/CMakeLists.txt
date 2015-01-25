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

#	Boost
#
SET(Boost_USE_STATIC_LIBS ON)
find_package( Boost COMPONENTS log program_options system date_time thread regex chrono REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

file(GLOB via  ${CMAKE_CURRENT_SOURCE_DIR}/via/http/*.cpp)
source_group("via" FILES ${via} )   


#
# sources
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
            ${via}
				wire.proto ${PROTO_SRCS} ${PROTO_HDRS} )

target_link_libraries(	${PROJECT} 
						${PROTOBUF_LIBRARIES}
						${Boost_LIBRARIES} )

#	client library


#	tests
