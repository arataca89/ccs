# `<io.h>` — Windows (I/O de baixo nível)

Funções específicas do Windows para I/O de baixo nível.
**Não existem no Linux/macOS** (lá, use `<unistd.h>` e
`<fcntl.h>`). Usado no `001_wc`, `002_cat`, `003_grep`,
`004_linked_list`.

## Terminais

### `_isatty`

**Header**: `<io.h>` (Windows)

**Assinatura**: `int _isatty(int fd);`

**O que faz**: equivalente a `isatty` (POSIX): retorna não-zero
se `fd` é um terminal.

**Retorno**: 1 se é terminal, 0 se não.

**Onde usamos**: `004` (via `#define isatty _isatty`).

**Cuidados**: é a versão Windows de `isatty`. O `#define isatty
_isatty` permite usar o mesmo nome no código.

**Ver também**: [`isatty`](unistd.md#isatty).

### `_fileno`

**Header**: `<io.h>` (Windows)

**Assinatura**: `int _fileno(FILE *stream);`

**O que faz**: equivalente a `fileno` (POSIX): retorna o
descritor de arquivo do stream.

**Retorno**: o descritor, ou -1 em erro.

**Onde usamos**: `004` (via `#define fileno _fileno`).

**Cuidados**: é a versão Windows de `fileno`.

**Ver também**: [`fileno`](unistd.md#fileno).

## Modo binário

### `_setmode`

**Header**: `<io.h>` (Windows)

**Assinatura**: `int _setmode(int fd, int mode);`

**O que faz**: muda o **modo** do descritor `fd` para `mode`
(`_O_BINARY` ou `_O_TEXT`). No modo binário, o Windows **não**
traduz `\r\n` ↔ `\n`.

**Retorno**: o modo anterior, ou -1 em erro.

**Onde usamos**: `001`, `002`, `003` (para configurar `stdin` e
`stdout` como binários).

**Cuidados**: sem isso, o Windows traduz `\r\n` para `\n` na
leitura, e a contagem de bytes fica errada. No Linux/macOS,
`_setmode` não existe (o modo é sempre binário).

**Ver também**: [`_O_BINARY`](fcntl.md#_o_binary).
