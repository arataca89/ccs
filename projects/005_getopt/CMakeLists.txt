cmake_minimum_required(VERSION 3.16)

project(ccs_getopt LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_executable(demo
    src/main.c
    src/getopt.c
)

target_include_directories(demo PRIVATE include)