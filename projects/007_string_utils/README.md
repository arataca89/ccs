# 007_string_utils: utilitários de string

> Implementação didática de funções de string que **não** estão
> no C padrão (ou que são POSIX), mas que todo programador C
> precisa.

## O que faz

Oferece 8 funções de string, cada uma implementada **do zero**:

| Função | O que faz |
|---|---|
| `strdup` | Duplica uma string (aloca + copia) |
| `strndup` | Duplica até N caracteres |
| `strtrim` | Remove espaços do início e do fim |
| `str_reverse` | Inverte a string in-place |
| `strtolower` | Converte para minúsculas in-place |
| `strtoupper` | Converte para maiúsculas in-place |
| `strsplit` | Divide a string por um delimitador |
| `strjoin` | Junta um array de strings com um separador |

E uma **CLI de demonstração** (`strings`) que exercita cada
função.

## Onde isso é usado na vida real

- **`strdup`**: todo programa C que precisa de uma cópia
  modificável de uma string (ex.: parsers, configuração,
  argumentos).
- **`strtrim`**: remover espaços de entrada do usuário (ex.:
  formulários, arquivos de configuração).
- **`strsplit`**: dividir CSV, linhas de comando, caminhos de
  arquivo.
- **`strjoin`**: construir strings a partir de partes (ex.: SQL,
  JSON, logs).
- **`str_reverse`**, **`strtolower`**, **`strtoupper`**:
  manipulação de texto em geral.

**Essas funções não estão no C padrão**, mas estão em **toda**
biblioteca de utilidades (GTK, GLib, APR, etc.). Implementá-las
do zero ensina como funcionam.

## Conceitos de C abordados

- **`malloc`/`free`** para alocação dinâmica de strings.
- **`memcpy`** para cópia de bytes (mais eficiente que `strcpy`).
- **Aritmética de ponteiros** (`end - start + 1` no `strtrim`,
  `p - start` no `strsplit`).
- **Array de ponteiros** (`char **` no `strsplit` e `strjoin`).
- **Terminação em `NULL`** (como `argv`).
- **Strings mutáveis vs. imutáveis** (por que `str_reverse`/
  `strtolower`/`strtoupper` exigem string mutável).
- **`strtol` com validação** (no `main`, para o `ndup`).
- **`unsigned char`** em `tolower`/`toupper` (para evitar UB).

## Como compilar

Requer CMake ≥ 3.16 e um compilador C11.

**Linux/macOS**:

    cmake -S . -B build
    cmake --build build

**Windows com TDM-GCC**:

    cmake -S . -B build -G "MinGW Makefiles"
    cmake --build build

## Como executar

**Da raiz do projeto** (`007_string_utils/`):

    build\strings.exe dup "ola"          (Windows)
    ./build/strings dup "ola"            (Linux/macOS)

**Comandos**:

    build\strings.exe dup "ola"
    build\strings.exe ndup 3 "ola mundo"
    build\strings.exe trim "  ola  "
    build\strings.exe rev "ola"
    build\strings.exe lower "OLA MUNDO"
    build\strings.exe upper "ola mundo"
    build\strings.exe split "a,b,c" ,
    build\strings.exe join ", " a b c
    build\strings.exe help

## Comandos

| Comando | O que faz |
|---|---|
| `dup STR` | Duplica STR |
| `ndup N STR` | Duplica os primeiros N caracteres de STR |
| `trim STR` | Remove espaços do início e fim de STR |
| `rev STR` | Inverte STR (in-place) |
| `lower STR` | Converte STR para minúsculas (in-place) |
| `upper STR` | Converte STR para maiúsculas (in-place) |
| `split STR DELIM` | Divide STR por DELIM (um caractere) |
| `join SEP STR...` | Junta STR... com SEP |
| `help` | Mostra a ajuda |

## Código de retorno

| Código | Significado |
|---|---|
| 0 | Sucesso |
| 1 | Erro (comando desconhecido, argumentos faltando, erro de alocação) |

## API

Ver [reference.md](reference.md) para a referência das funções
(assinatura, retorno, quem libera a memória, exemplos, cuidados).

O `reference.md` começa com uma seção sobre **strings mutáveis
vs. imutáveis**, que é essencial para usar as funções in-place
(`str_reverse`, `strtolower`, `strtoupper`).

## Testes

Ver [tests/README.md](tests/README.md). Os testes são **comandos
de linha** (não há fixtures de arquivo), com gabaritos medidos.

## Notas de implementação

Ver [notes.md](notes.md). Cobre as decisões (`malloc`/`free`,
`memcpy`, aritmética de ponteiros, o conflito de `strrev` com o
MinGW, `join` com `&argv[3]`).

## Explicação detalhada

Ver [explained.md](explained.md). Explicação **linha por linha**
do `strings.c` e do `main.c`.

## Conceitos relacionados

- [Aritmética de ponteiros](../../docs/pointer-arithmetic.md) —
  usada no `strtrim` e no `strsplit`.
- [Especificadores de formato do `printf`](../../docs/format-specifiers.md)
  — usados no `main.c`.
- [Strings mutáveis vs. imutáveis](reference.md#strings-mutáveis-vs-imutáveis)
  — por que as funções in-place exigem string mutável.
- [Padrões de C](../../docs/patterns.md) — o padrão
  `if (!ptr)` para verificação de alocação.

## Referências

- POSIX `strdup`: <https://pubs.opengroup.org/onlinepubs/9699919799/functions/strdup.html>
- POSIX `strndup`: <https://pubs.opengroup.org/onlinepubs/9699919799/functions/strndup.html>
- *The C Programming Language*, Kernighan & Ritchie — capítulo 5
  (pointers and arrays) e 7 (I/O).
- *Modern C*, Jens Gustedt — capítulo sobre strings.

---

arataca89@gmail.com

Aulas de programação
