cmake_minimum_required(VERSION 2.6)
project(legacycuramachineconverter)

SET(CMAKE_CXX_FLAGS "-std=c++11")

add_executable(legacycuramachineconverter main.cpp)

install(TARGETS legacycuramachineconverter RUNTIME DESTINATION bin)
