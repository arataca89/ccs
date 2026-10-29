cmake_minimum_required(VERSION 3.16)

project(ccs_cat LANGUAGES C)

# C11 portável. MSVC não entende "C11" como string exata, então usamos
# a feature c_std_11, que o CMake traduz para o compilador certo.
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)   # garante C padrão, sem extensões GNU

# Avisos de compilação agressivos, mas portáveis.
if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_executable(cat
    src/main.c
    src/cat.c
)

target_include_directories(cat PRIVATE include)