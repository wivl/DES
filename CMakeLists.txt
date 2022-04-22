cmake_minimum_required(VERSION 3.21.1)
project(des)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} main.c des.c)
