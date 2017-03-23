project(helloworld)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_CXX_FLAGS "-Wall -pthread -static-libstdc++ -DCPPTEST -DBY_THREAD -std=c++11")
#cmake -DCMAKE_BUILD_TYPE=debug ../
set(CMAKE_CXX_FLAGS_DEBUG "-g3")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

set(COMMON_SRCS
	wrapperfun.cc
  thread_pool.cc
  epoll_util.cc
  buff.cc
  cache.cc
  http_request.cc
  http_handler.cc
  )

add_executable(flashwebserver flashwebserver.cc ${COMMON_SRCS})
add_executable(tests tests.cc ${COMMON_SRCS})
