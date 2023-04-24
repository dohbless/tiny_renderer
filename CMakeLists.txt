cmake_minimum_required(VERSION 3.23)
project(TINY_rendered)

set(CMAKE_CXX_STANDARD 14)

add_executable(TINY_rendered main.cpp tgaimage.h tgaimage.cpp model.h model.cpp geometry.h)
