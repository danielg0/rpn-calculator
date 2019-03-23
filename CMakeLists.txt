cmake_minimum_required (VERSION 3.10)
project (cpp-rpn)

include_directories (include)
file (GLOB cpp-rpn_SRC "src/*.cpp")

add_executable (cpp-rpn ${cpp-rpn_SRC})
