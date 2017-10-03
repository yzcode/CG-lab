cmake_minimum_required (VERSION 3.0)
project (CG-lab1)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "-Wall -framework GLUT -framework OpenGL -lglog -std=c++14 -Wno-deprecated-declarations -Wno-missing-braces")

add_library(SystemDataStructure SystemDS.cpp)

add_executable(main main.cpp)
target_link_libraries (main SystemDataStructure)
