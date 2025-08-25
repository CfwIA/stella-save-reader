cmake_minimum_required(VERSION 3.10.0)
project(para-savefile-parser VERSION 0.1.0 LANGUAGES C CXX)
set (CMAKE_CXX_STANDARD 23)
add_executable(para-savefile-parser source.cpp Parser.cpp)

target_link_libraries(para-savefile-parser unarr)
