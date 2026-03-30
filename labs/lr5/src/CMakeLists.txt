cmake_minimum_required(VERSION 3.15)
project(lr5) 

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3.0.2 COMPONENTS Graphics Window System REQUIRED)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3 -march=native -mtune=native -funroll-loops -ffast-math -flto)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-O0 -g -Wall -Wextra -Wpedantic)
endif()

add_executable(lr5 main.cpp live.cpp pattern.cpp neural.cpp organism.cpp base.cpp grid.cpp light.cpp)

target_link_libraries(lr5 
    SFML::Graphics
    SFML::Window
    SFML::System
)

add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)