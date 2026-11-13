# `<fcntl.h>` — controle de arquivos

Constantes para controle de arquivos. No Windows, define
`_O_BINARY` (usado com `_setmode`). No Linux/macOS, define
`O_RDONLY`, `O_WRONLY`, `O_CREAT`, etc. (não usamos).

## Modos de arquivo (Windows)

### `_O_BINARY`

**Header**: `<fcntl.h>` (Windows)

**O que é**: uma macro que representa o **modo binário** para
`_setmode`. No modo binário, o Windows **não** traduz `\r\n` ↔
`\n`.

**Onde usamos**: `001`, `002`, `003` (com `_setmode`).

**Cuidados**: no Linux/macOS, não existe (o modo é sempre
binário). Para portabilidade, use `#ifdef _WIN32`.

**Ver também**: [`_setmode`](io.md#_setmode).

### `_O_TEXT`

**Header**: `<fcntl.h>` (Windows)

**O que é**: uma macro que representa o **modo texto** para
`_setmode`. No modo texto, o Windows traduz `\r\n` ↔ `\n`.

**Onde usamos**: **não usamos** (o padrão do Windows já é texto,
e nós mudamos para binário).

**Cuidados**: no Linux/macOS, não existe.

**Ver também**: [`_setmode`](io.md#_setmode), [`_O_BINARY`](#_o_binary).
