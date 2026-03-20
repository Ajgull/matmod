cmake_minimum_required(VERSION 3.15)
project(lr4)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Находим Python и pybind11
find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

# Находим OpenMP
find_package(OpenMP REQUIRED)

# Добавляем исполняемый файл
add_executable(lr4 main.cpp)

# Линкуем библиотеки
target_link_libraries(lr4 
    PRIVATE 
    pybind11::embed
    OpenMP::OpenMP_CXX
)

# Добавляем флаги компиляции
target_compile_options(lr4 PRIVATE ${OpenMP_CXX_FLAGS})

# Копируем Python файлы в директорию сборки (корень build)
add_custom_command(TARGET lr4 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/main.py
        ${CMAKE_BINARY_DIR}/main.py
    COMMENT "Copying main.py to build directory"
)

# Устанавливаем
install(TARGETS lr4 DESTINATION bin)
install(FILES main.py DESTINATION bin)