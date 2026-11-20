cmake_minimum_required(VERSION 3.16)

project(ccs_string_utils LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_executable(strings
    src/main.c
    src/strings.c
)

target_include_directories(strings PRIVATE include)