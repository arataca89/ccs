cmake_minimum_required(VERSION 3.16)

project(ccs_snake LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Define -std=gnu99 no GCC/Clang
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS ON)

add_executable(snake
    src/main.c
    src/snake.c
    src/terminal.c
)

target_include_directories(snake PRIVATE include)