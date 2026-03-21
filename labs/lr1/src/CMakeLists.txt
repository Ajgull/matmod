cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenMP REQUIRED)

add_executable(lr1 main.cpp solver.cpp spacecraft_solver.cpp state.cpp)

target_link_libraries(lr1 PRIVATE OpenMP::OpenMP_CXX)