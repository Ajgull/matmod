cmake_minimum_required(VERSION 3.15)
project(lr5) 

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_path(SFML_INCLUDE_DIR SFML/Graphics.hpp)
find_library(SFML_GRAPHICS_LIBRARY sfml-graphics)
find_library(SFML_WINDOW_LIBRARY sfml-window)
find_library(SFML_SYSTEM_LIBRARY sfml-system)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3 -march=native -mtune=native -funroll-loops -ffast-math -flto)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-O0 -g -Wall -Wextra -Wpedantic)
endif()

add_executable(lr5 main.cpp live.cpp pattern.cpp neural.cpp organism.cpp)
target_include_directories(lr5 PRIVATE ${SFML_INCLUDE_DIR})
add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
target_link_libraries(lr5 
    ${SFML_GRAPHICS_LIBRARY}
    ${SFML_WINDOW_LIBRARY}
    ${SFML_SYSTEM_LIBRARY}
)