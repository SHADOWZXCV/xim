cmake_minimum_required(VERSION 3.11)

project(XIM C)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SRC src/*.c)

add_executable(xim main.c ${SRC})

# include directory
target_include_directories(xim PRIVATE ${CMAKE_SOURCE_DIR}/include)
