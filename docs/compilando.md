# 🛠️ Compilando um projeto do CCS com CMake

## Passos

1. Navegue até a pasta do projeto no terminal.
2. Verifique se existe o arquivo `CMakeLists.txt`. Se não existir, crie
   (ver template abaixo).
3. Compile (ver seção "Compilação").

## Template de `CMakeLists.txt` para projetos do CCS

Todos os projetos do CCS usam o mesmo `CMakeLists.txt`. Apenas
**três partes variam** entre projetos: o nome do projeto, o nome do
executável e a lista de arquivos `.c`. Estas partes estão marcadas
com `# ALTERAR` no template abaixo.

### O template

```cmake
cmake_minimum_required(VERSION 3.16)

# ALTERAR: nome lógico do projeto (convenção: ccs_<nome>)
project(ccs_NOME LANGUAGES C)

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

# ALTERAR: nome do executável e lista de arquivos .c
add_executable(NOME
    src/main.c
    src/MODULO1.c
    src/MODULO2.c
)

# ALTERAR: mesmo nome do add_executable acima
target_include_directories(NOME PRIVATE include)
```

### As três partes que variam

| Onde | O que trocar | Exemplo (`003_grep`) |
|---|---|---|
| `project(...)` | `ccs_NOME` | `ccs_grep` |
| `add_executable(...)` | `NOME` e a lista de `.c` | `grep`, com `src/main.c` e `src/grep.c` |
| `target_include_directories(...)` | `NOME` (o **mesmo** do `add_executable`) | `grep` |

**Atenção ao duplo uso do nome do executável**: ele aparece em
`add_executable(...)` **e** em `target_include_directories(...)`.
Se você mudar em um lugar e esquecer do outro, o build quebra (ou
pior: o include path não é aplicado, e o compilador não acha os
headers em `include/`).

**Erro comum**: copiar o `CMakeLists.txt` do `002_cat` para o
`003_grep` e esquecer de trocar o `cat` no
`target_include_directories`. O build do `grep` então não acha
`include/grep.h`, e a mensagem de erro não aponta para o
`CMakeLists.txt` — aponta para o `#include`, o que confunde.

### O que **não** muda

Estas partes são idênticas em todos os projetos do CCS:

- `cmake_minimum_required(VERSION 3.16)`
- `LANGUAGES C` (só C, não C++)
- `set(CMAKE_C_STANDARD 11)` e as duas linhas seguintes
- O bloco `if (MSVC) ... else() ... endif()` com os warnings
- `target_include_directories(... PRIVATE include)` (só o nome
  do executável muda)

## Compilação

**Windows com TDM-GCC** (ou MinGW):

```cmd
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
```

O `-G "MinGW Makefiles"` diz ao CMake para preparar a compilação
usando o TDM-GCC (que é baseado em MinGW), em vez de tentar usar o
Visual Studio por padrão.

**Linux/macOS** (GCC ou Clang):

```bash
cmake -S . -B build
cmake --build build
```

**Windows com MSVC** (Visual Studio Build Tools):

```cmd
cmake -S . -B build
cmake --build build
```

Se tudo correr bem, o executável estará em `build/` (Linux/macOS)
ou `build/<nome>.exe` (Windows).

## Se o build falhar com "header not found"

O problema é quase sempre o `target_include_directories` com o
nome errado. Verifique se o nome bate com o `add_executable`.
