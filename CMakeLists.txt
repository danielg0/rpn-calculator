cmake_minimum_required (VERSION 3.10)
project (rpn-calculator)

find_package(SFML 2 REQUIRED system window graphics network audio)
find_package(TGUI 0.8 REQUIRED)

include_directories (include)
file (GLOB rpn-calculator_SRC "src/*.cpp")

add_executable (rpn-calculator ${rpn-calculator_SRC})
target_link_libraries(rpn-calculator sfml-graphics sfml-audio tgui)
