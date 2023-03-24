cmake_minimum_required(VERSION 3.23)
project(lab3 C)

set(CMAKE_C_STANDARD 23)

add_executable(lab3 main.c)

target_link_libraries(lab3 m)
