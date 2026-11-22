# Compilando um projeto do CCS

Como compilar os projetos do CCS em Windows, Linux e macOS.

## Visão geral

O CCS usa **C11** e **CMake** (≥ 3.16). O compilador recomendado é o
**GCC** (ou Clang, no macOS). A partir do projeto `009`, o CCS passou
a usar o **GCC 16.x** (via MSYS2 no Windows), que suporta C23 e tem
melhor desempenho.

| Plataforma | Compilador | Como obter |
|---|---|---|
| **Windows** | GCC 16.x (MinGW-w64/UCRT64) | MSYS2 (recomendado) |
| **Linux** | GCC (já vem na distro) | `build-essential` (Debian/Ubuntu) |
| **macOS** | Clang (disfarçado de `gcc`) ou GCC real | Homebrew (opcional) |

## Windows: MSYS2 + GCC + Ninja

### Por que MSYS2?

O MSYS2 é a forma mais fácil de obter um **GCC moderno** no
Windows . Ele fornece builds nativos do GCC, CMake,
Ninja, e outras ferramentas, com um gerenciador de pacotes
(`pacman`) que mantém tudo atualizado.

**Atenção**: existem três terminais no MSYS2 (`MSYS`, `MINGW64`,
`UCRT64`). Para compilar projetos **nativos do Windows** (como os
do CCS), use o terminal **UCRT64**. Os outros usam o subsistema
POSIX do MSYS2, e podem causar conflito de caminhos .

### Passo 1: Instalar o MSYS2

1. Baixe o instalador em <https://www.msys2.org/>.
2. Execute o instalador. O caminho padrão (`C:\msys64`) é
   recomendado .
3. Ao final, o terminal **UCRT64** abre automaticamente.

### Passo 2: Instalar o GCC e o Ninja

No terminal UCRT64, execute:

```bash
pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-ninja
```

Isso instala o GCC 16.x e o Ninja .

**Verificação**:

```bash
gcc --version
# gcc.exe (Rev3, Built by MSYS2 project) 16.2.0

ninja --version
# 1.12.1 (ou similar)
```

### Passo 3: Adicionar o MSYS2 ao PATH do Windows (opcional)

Se você quer usar o `gcc` e o `ninja` **fora** do terminal UCRT64
(no `cmd` ou no PowerShell), adicione `C:\msys64\ucrt64\bin` ao
PATH do sistema .

**Atenção**: **remova** o TDM-GCC do PATH (se estiver lá), senão
haverá conflito de versões. O `where gcc` deve mostrar **apenas**
o caminho do MSYS2 .

**Verificação** (no `cmd` ou PowerShell, **não** no terminal
UCRT64):

```cmd
where gcc
```

Deve mostrar `C:\msys64\ucrt64\bin\gcc.exe` (e **não** o
caminho do TDM-GCC).

> ⚠️ Se ainda mostrar o TDM-GCC, tente desinstalar o TDM-GCC.

### Se você tem o TDM-GCC instalado

O TDM-GCC é uma distribuição antiga do GCC (parada na versão
10.3). Se você o tem instalado, **desinstale** (Painel de
Controle → Programas → Desinstalar). Apenas remover do PATH
**não** é suficiente: o `gcc.exe` pode ficar "escondido" em
`C:\Windows` ou em pastas do sistema, e o `cmd` continua
encontrando.

**Depois de desinstalar**, confirme com:

```cmd
where gcc
```

### Passo 4: Compilar com CMake + Ninja

```cmd
cmake -G Ninja -S . -B build
cmake --build build
```

O `-G Ninja` diz ao CMake para gerar os arquivos de build para o
Ninja (em vez do Make). O Ninja é mais rápido que o `make`
tradicional, especialmente em projetos grandes .

**Nota**: para os projetos pequenos do CCS, a diferença de
velocidade é pequena. Mas o Ninja é o padrão recomendado pelo
CMake para projetos modernos, e prepara o aluno para o que ele
encontrará em projetos profissionais .

## Linux: GCC já vem na distro

A maioria das distribuições Linux já vem com o GCC (ou com o
pacote `build-essential`, que inclui GCC, Make, e outras
ferramentas).

**Verificação**:

```bash
gcc --version
```

Se não estiver instalado:

```bash
# Debian/Ubuntu
sudo apt install build-essential

# Fedora
sudo dnf install gcc make

# Arch
sudo pacman -S gcc make
```

**Compilar**:

```bash
cmake -S . -B build
cmake --build build
```

**Ninja no Linux** (opcional):

```bash
sudo apt install ninja-build   # Debian/Ubuntu
cmake -G Ninja -S . -B build
cmake --build build
```

## macOS: o `gcc` é o Clang (e como instalar o GCC real)

**Pegadinha**: o `gcc` que vem no macOS **não é o GCC de
verdade**. É o **Clang disfarçado**. A Apple chama o Clang de
`gcc` por compatibilidade histórica.

**Para o CCS**, o Clang (disfarçado de `gcc`) **funciona** para
C11/C17. Você pode compilar normalmente:

```bash
cmake -S . -B build
cmake --build build
```

**Se você quiser o GCC real** (para testar C23 ou recursos
específicos do GCC), instale via Homebrew:

```bash
brew install gcc
```

O binário fica em `/opt/homebrew/bin/gcc-15` (ou similar). Para
usar, chame pelo nome completo:

```bash
cmake -DCMAKE_C_COMPILER=gcc-15 -S . -B build
```

**Ninja no macOS**:

```bash
brew install ninja
cmake -G Ninja -S . -B build
cmake --build build
```

## O template de `CMakeLists.txt`

Todos os projetos do CCS usam o mesmo `CMakeLists.txt`. Apenas
**três partes variam**: o nome do projeto, o nome do executável
e a lista de arquivos `.c`. Estas partes estão marcadas com
`# ALTERAR` no template abaixo.

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
)

# ALTERAR: mesmo nome do add_executable acima
target_include_directories(NOME PRIVATE include)
```

### As três partes que variam

| Onde | O que trocar | Exemplo (`008_rle`) |
|---|---|---|
| `project(...)` | `ccs_NOME` | `ccs_rle` |
| `add_executable(...)` | `NOME` e a lista de `.c` | `rle`, com `src/main.c` e `src/rle.c` |
| `target_include_directories(...)` | `NOME` (o **mesmo** do `add_executable`) | `rle` |

**Atenção ao duplo uso do nome do executável**: ele aparece em
`add_executable(...)` **e** em `target_include_directories(...)`.
Se você mudar em um lugar e esquecer do outro, o build quebra (ou
pior: o include path não é aplicado, e o compilador não acha os
headers em `include/`).

## Compilando com Ninja

**Windows (terminal UCRT64 ou cmd/PowerShell)**:

```cmd
cmake -G Ninja -S . -B build
cmake --build build
```

**Linux/macOS**:

```bash
cmake -G Ninja -S . -B build
cmake --build build
```

**Se você não tem Ninja**: use o gerador padrão (Make no
Linux/macOS, MinGW Makefiles no Windows):

```cmd
cmake -S . -B build -G "MinGW Makefiles"   (Windows)
cmake -S . -B build                         (Linux/macOS)
cmake --build build
```

## Se o build falhar com "header not found"

O problema é quase sempre o `target_include_directories` com o
nome errado. Verifique se o nome bate com o `add_executable`.

---

arataca89@gmail.com

Aulas de programação
