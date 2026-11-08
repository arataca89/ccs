cmake_minimum_required(VERSION 3.16)

project(ccs_linked_list LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_executable(list
    src/main.c
    src/list.c
)

target_include_directories(list PRIVATE include)