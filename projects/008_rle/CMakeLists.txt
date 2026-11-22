cmake_minimum_required(VERSION 3.16)

project(ccs_rle LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_executable(rle
    src/main.c
    src/rle.c
)

target_include_directories(rle PRIVATE include)