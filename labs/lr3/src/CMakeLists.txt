cmake_minimum_required(VERSION 3.15)



set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3)
    add_compile_options(-march=native)
    add_compile_options(-mtune=native)
    add_compile_options(-funroll-loops)
    add_compile_options(-ffast-math)
    add_compile_options(-flto)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-O0)                     # Без оптимизации
    add_compile_options(-g)                      # Отладочная информация
    add_compile_options(-Wall -Wextra -Wpedantic) # Все предупреждения
endif()

add_executable(lr3 main.cpp)

