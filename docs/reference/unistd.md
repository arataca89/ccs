# `<unistd.h>` — POSIX (Unix)

Funções POSIX (não C padrão) para interagir com o sistema
operacional. **Não existe no Windows/MSVC** (no MinGW, existe
parcialmente). Usado no `004_linked_list` (`isatty`, `fileno`).

## Terminais

### `isatty`

**Header**: `<unistd.h>` (POSIX)

**Assinatura**: `int isatty(int fd);`

**O que faz**: retorna não-zero se o descritor de arquivo `fd`
é um **terminal**, zero caso contrário.

**Retorno**: 1 se é terminal, 0 se não.

**Onde usamos**: `004` (`main`, para detectar modo interativo
vs. batch).

**Cuidados**: **não é C padrão** (é POSIX). No Windows, use
`_isatty` (em `<io.h>`). Ver [io.md](io.md#_isatty).

**Ver também**: [`fileno`](#fileno), [`_isatty`](io.md#_isatty).

### `fileno`

**Header**: `<unistd.h>` (POSIX)

**Assinatura**: `int fileno(FILE *stream);`

**O que faz**: retorna o **descritor de arquivo** associado ao
stream (um `int`). No Unix, `stdin` é 0, `stdout` é 1, `stderr`
é 2.

**Retorno**: o descritor, ou -1 em erro.

**Onde usamos**: `004` (`main`, para passar para `isatty`).

**Cuidados**: **não é C padrão** (é POSIX). No Windows, use
`_fileno` (em `<io.h>`). Ver [io.md](io.md#_fileno).

**Ver também**: [`isatty`](#isatty), [`_fileno`](io.md#_fileno).
