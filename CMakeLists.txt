cmake_minimum_required(VERSION 3.25)
project(PEA_3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXE_LINKER_FLAGS "-static")

add_executable(PEA_3 main.cpp Graf.cpp Graf.h Menu.cpp Menu.h Genetyczny.cpp Genetyczny.h)
