# 005_getopt: parsing de flags de linha de comando

> Implementação didática do `getopt` POSIX: parsing de flags
> curtas, agrupadas, com argumento, e `--`.

## O que faz

Implementa a função `getopt`, que faz parsing de flags de linha
de comando. O `demo` (o `main.c`) usa o `getopt` para processar
`-v`, `-o`, `-h`, e mostra os resultados.

```
$ demo -vo saida.txt arquivo1 arquivo2
verbose = 1
output  = saida.txt
arquivos:
  arquivo1
  arquivo2
```

## Onde isso é usado na vida real

- **Todo programa Unix** que aceita flags usa `getopt` (ou
  `getopt_long`): `ls`, `cp`, `grep`, `sed`, `awk`, `tar`, `git`
  (internamente).
- **`getopt` é POSIX**, e está em `<unistd.h>` no Linux/macOS.
  No Windows, MinGW tem, mas MSVC **não** (precisa de
  implementação própria, como esta).
- A ideia "parsing de flags" é a base de **qualquer** CLI. Mesmo
  linguagens modernas (Python `argparse`, Rust `clap`, Go `flag`)
  têm equivalentes.

## Conceitos de C abordados

- **Variáveis globais** (`optarg`, `optind`, `optopt`, `opterr`):
  o estado do `getopt` é global (POSIX).
- **`extern`**: declaração de globais em um header, definição no
  `.c`.
- **Variáveis `static` locais**: o estado interno (`nextchar`)
  persiste entre chamadas.
- **`strchr`**: para procurar a flag na `optstring`.
- **Parsing de flags**: agrupadas (`-vo`), com argumento colado
  (`-osaida.txt`), com argumento separado (`-o saida.txt`).
- **`--`**: para o parsing de flags.
- **`-` sozinho**: não é flag (é stdin, por convenção).
- **Mensagens de erro** com `opterr`.
- **A limitação de thread-safety**: globais não são thread-safe.

## Como compilar

Requer CMake ≥ 3.16 e um compilador C11.

**Linux/macOS**:

    cmake -S . -B build
    cmake --build build

**Windows com TDM-GCC**:

    cmake -S . -B build -G "MinGW Makefiles"
    cmake --build build

## Como executar

**Da raiz do projeto** (`005_getopt/`):

    build\demo.exe -v                       (Windows)
    ./build/demo -v                         (Linux/macOS)

**Com flags**:

    build\demo.exe -vv
    build\demo.exe -o saida.txt
    build\demo.exe -osaida.txt
    build\demo.exe -vo saida.txt arquivo1 arquivo2
    build\demo.exe -v -- arquivo1
    build\demo.exe -h

## Flags da demo

| Flag | O que faz |
|---|---|
| `-v` | Modo verboso (pode ser repetida: `-vv`) |
| `-o ARQUIVO` | Arquivo de saída (aceita `-o arquivo` ou `-oarquivo`) |
| `-h` | Mostra ajuda e sai |

**Flags agrupadas**: `-vo saida.txt` é o mesmo que `-v -o saida.txt`.
**Argumento colado**: `-osaida.txt` é o mesmo que `-o saida.txt`.
**`--`**: para o parsing (o resto são argumentos).
**`-` sozinho**: não é flag.

## Interface do `getopt`

```c
int getopt(int argc, char *const argv[], const char *optstring);
```

**Variáveis globais** (POSIX):

| Variável | O que é |
|---|---|
| `optarg` | O argumento da flag (se ela espera um) |
| `optind` | Índice do próximo argumento não-flag |
| `optopt` | A flag inválida (quando `getopt` retorna `'?'`) |
| `opterr` | Se 0, `getopt` não imprime erros (padrão: 1) |

**Retorno**:

- Caractere da flag (`'v'`, `'o'`) — flag válida.
- `-1` — fim das flags.
- `'?'` — flag inválida, ou argumento faltando.

## `optstring`: como descrever as flags

A `optstring` é uma string que descreve as flags:

- `"v"` — flag `v` sem argumento.
- `"vo:"` — flag `v` sem argumento, flag `o` **com** argumento.
- `":vo:"` — o `:` no início muda o comportamento de erro
  (retorna `:` em vez de `?` para argumento faltando). **Não
  implementado**.

## Código de retorno do demo

| Código | Significado |
|---|---|
| 0 | Sucesso (ou `-h`) |
| 1 | Flag inválida, ou argumento faltando |

## Testes

Ver [tests/README.md](tests/README.md). Os testes são **comandos
de linha** (não há fixtures de arquivo), com gabaritos medidos.

## Notas de implementação

Ver [notes.md](notes.md). Cobre o algoritmo do `getopt`, o estado
`static`, as globais POSIX, a limitação thread-safe, e as
decisões de escopo.

## Conceitos relacionados

- [Design de CLI](../../docs/cli-design.md) — flags, comandos e
  modo interativo; por que o CCS usa parsing manual (e este
  projeto implementa o `getopt` que o CCS poderia usar).
- [Por que usar funções `static`](../../docs/static.md) — o
  estado interno do `getopt` (`nextchar`) é uma variável `static`
  local (uso 3 de `static`).

## Referências

- POSIX `getopt`:
  <https://pubs.opengroup.org/onlinepubs/9699919799/functions/getopt.html>
- GNU `getopt`:
  <https://www.gnu.org/software/libc/manual/html_node/Getopt.html>
- *The GNU C Library Reference Manual* — capítulo "Parsing
  Program Arguments".

---

arataca89@gmail.com

Aulas de programação
