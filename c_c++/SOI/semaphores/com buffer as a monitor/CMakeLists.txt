cmake_minimum_required(VERSION 3.24)
project(lab4)

set(CMAKE_CXX_STANDARD 17)

add_executable(lab4 main.cpp ingreedients_buffer.cpp ingreedients_buffer.h consumer.h producer.h utils.h)
