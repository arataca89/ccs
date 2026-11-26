# Funções de terminal dependentes de plataforma

> As funções que manipulam o terminal (tamanho, cores, input,
> sleep) são **específicas de plataforma**. Este texto explica
> cada uma, e mostra como isolar a portabilidade com `#ifdef`.

## O problema: o terminal é diferente em cada plataforma

O terminal (a "janela" onde o programa roda) tem **comportamentos
diferentes** em cada plataforma:

| O que | Windows | Linux/macOS |
|---|---|---|
| **Tamanho** | `GetConsoleScreenBufferInfo` | `ioctl` + `TIOCGWINSZ` |
| **Cores ANSI** | Precisa `SetConsoleMode` (modo VT) | Nativo |
| **Ler tecla sem Enter** | `_getch` (`conio.h`) | `termios` (modo não-canônico) |
| **Verificar se há tecla** | `_kbhit` | `read` com `VMIN = 0` |
| **Sleep** | `Sleep` (ms) | `usleep` (µs) |
| **Restaurar terminal** | (não precisa) | `tcsetattr` |

**A solução**: isolar cada função em um `#ifdef _WIN32`, e
implementar **duas versões** (Windows e Linux/macOS). O resto do
código **não sabe** qual está sendo usado.

## `get_terminal_size`

### O que faz

Obtém o tamanho do terminal (colunas e linhas).

### Windows

```c
CONSOLE_SCREEN_BUFFER_INFO csbi;
if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
    return -1;
*cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
*rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
```

**`GetConsoleScreenBufferInfo`**: retorna informações sobre o
**buffer** do console.

**`srWindow`**: a **janela visível** (o que o usuário vê). O
buffer pode ser **maior** que a janela (por causa do scroll), mas o
que importa é a janela visível.

**`Right - Left + 1`**: o número de colunas (o `+1` é porque o
`Right` e o `Left` são inclusivos).

**`Bottom - Top + 1`**: o número de linhas.

### Linux/macOS

```c
struct winsize w;
if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1)
    return -1;
*cols = w.ws_col;
*rows = w.ws_row;
```

**`ioctl`**: uma chamada de sistema que controla dispositivos.

**`TIOCGWINSZ`**: "get window size" (obter o tamanho da janela).

**`struct winsize`**: tem `ws_col` (colunas) e `ws_row` (linhas).

**`STDOUT_FILENO`**: o descritor do `stdout` (1).

### Retorno

**0** em sucesso, **-1** se não for um terminal (ex.: `stdout`
redirecionado para um arquivo).

## `enable_ansi`

### O que faz

Habilita o modo ANSI (Virtual Terminal) no Windows.

### Windows

```c
HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
DWORD mode = 0;
if (GetConsoleMode(h, &mode)) {
    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(h, mode);
}
```

**`GetConsoleMode`**: obtém o modo atual do console.

**`ENABLE_VIRTUAL_TERMINAL_PROCESSING`**: a flag que liga o modo
VT (interpretação de ANSI).

**`SetConsoleMode`**: aplica o novo modo.

### Linux/macOS

**No-op** (o terminal já interpreta ANSI nativamente).

### Por que é necessário

No Windows 10+, o console **não** interpreta ANSI por padrão. Sem o
`enable_ansi`, os códigos `\033[92m` aparecem **literalmente** na
tela (como `←[92m`), em vez de mudar a cor.

## `read_key`

### O que faz

Lê uma tecla **sem bloquear** (não-bloqueante).

### Windows

```c
if (!_kbhit())
    return -1;
int c = _getch();
if (c == 0 || c == 0xE0) {
    int code = _getch();
    switch (code) {
    case 72: return KEY_UP;
    ...
    }
}
return c;
```

**`_kbhit`**: verifica se há tecla **sem remover** (não-bloqueante).

**`_getch`**: lê a tecla **sem eco**.

**Teclas especiais**: as setas enviam **2 bytes** (`0` ou `0xE0` +
código). O `read_key` lê o primeiro; se for `0`/`0xE0`, lê o
segundo.

### Linux/macOS

```c
unsigned char c;
if (read(STDIN_FILENO, &c, 1) != 1)
    return -1;
if (c == 27) {   // ESC
    unsigned char seq[2];
    read(STDIN_FILENO, &seq[0], 1);   // '['
    read(STDIN_FILENO, &seq[1], 1);   // 'A', 'B', 'C', ou 'D'
    if (seq[0] == '[') {
        switch (seq[1]) {
        case 'A': return KEY_UP;
        ...
        }
    }
}
return c;
```

**`read`**: lê bytes do `stdin`. Com `VMIN = 0` (configurado no
`setup_terminal`), é **não-bloqueante**.

**Teclas especiais**: as setas enviam **3 bytes** (`ESC` + `[` +
letra). O `read_key` lê o `ESC`; se for `ESC`, lê os dois
seguintes.

### `setup_terminal` (Linux/macOS)

```c
tcgetattr(STDIN_FILENO, &old_termios);
struct termios new_termios = old_termios;
new_termios.c_lflag &= ~(ICANON | ECHO);
new_termios.c_cc[VMIN] = 0;
new_termios.c_cc[VTIME] = 0;
tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
```

**`tcgetattr`**: obtém o estado atual (salva em `old_termios`).

**`ICANON`**: modo canônico (desliga).

**`ECHO`**: eco (desliga).

**`VMIN = 0`**: leitura não-bloqueante.

**`VTIME = 0`**: sem timeout.

**`tcsetattr`**: aplica.

## `sleep_ms`

### O que faz

Dorme por `ms` milissegundos.

### Windows

```c
Sleep((DWORD)ms);
```

**`Sleep`**: dorme por `ms` milissegundos.

### Linux/macOS

```c
usleep((useconds_t)ms * 1000);
```

**`usleep`**: dorme por **microssegundos**. O `* 1000` converte ms
para µs.

**Nota**: `usleep` é POSIX, não C padrão. Com C11 estrito, não
compila no Linux. Solução: C99 + extensions ON, ou `nanosleep`,
ou `#define _DEFAULT_SOURCE`.

## `restore_terminal`

### O que faz

Restaura o terminal ao estado original (Linux/macOS).

### Windows

**No-op** (o `_getch` não muda o terminal permanentemente).

### Linux/macOS

```c
if (termios_saved) {
    tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
    termios_saved = 0;
}
```

**`tcsetattr`**: aplica o estado salvo (`old_termios`).

### Por que é necessário

O `setup_terminal` desligou `ICANON` e `ECHO`. Se o programa
terminar sem restaurar, o terminal fica em **modo não-canônico**
(sem eco, sem Enter), e o usuário precisa **reiniciar** o terminal
(ou rodar `reset`).

**Por isso** o `restore_terminal` é chamado **no final** do
programa (e em handlers de `SIGINT`, se houver).

## Como isolar a portabilidade

**A estrutura**:

```c
#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#endif

/* ... */

int get_terminal_size(int *cols, int *rows)
{
#ifdef _WIN32
    /* versão Windows */
#else
    /* versão Linux/macOS */
#endif
}
```

**O resto do código** (o `main.c`, o `snake.c`) **não** tem
`#ifdef`. Ele chama `get_terminal_size`, `read_key`, etc., e
**não sabe** qual versão está sendo usada.

**Isso é o isolamento**: a portabilidade fica **concentrada** no
`terminal.c`, e o resto do código é **portável**.

## O que **não** isolar

**Não** coloque `#ifdef` no `snake.c` ou no `main.c`. A lógica do
jogo é **portável** (não depende de plataforma). Só as funções de
terminal precisam de `#ifdef`.

**Exceção**: o `main.c` pode ter `#ifdef _WIN32` para o
`_setmode` (configurar `stdin`/`stdout` como binários). Mas isso é
**uma linha** (não a lógica).

## Resumo

| Função | Windows | Linux/macOS |
|---|---|---|
| **Tamanho** | `GetConsoleScreenBufferInfo` | `ioctl` + `TIOCGWINSZ` |
| **ANSI** | `SetConsoleMode` (modo VT) | (nativo) |
| **Ler tecla** | `_kbhit` + `_getch` | `read` + `termios` |
| **Sleep** | `Sleep` (ms) | `usleep` (µs) |
| **Restaurar** | (no-op) | `tcsetattr` |

**Regras**:

1. **Isole a portabilidade** no `terminal.c` (não no resto).
2. **Use `#ifdef _WIN32`** para as diferenças.
3. **Implemente as duas versões** (Windows e Linux/macOS).
4. **O resto do código** chama as funções, sem `#ifdef`.
5. **Sempre** restaure o terminal no final (Linux/macOS).

## Onde isso aparece no CCS

- **`009_snake`**: o `terminal.c` tem todas essas funções.
- **`001_wc`**, **`002_cat`**, **`003_grep`**: usam `_setmode`
  (Windows) para configurar `stdin`/`stdout` como binários.
- **`004_linked_list`**: usa `isatty`/`fileno` (POSIX) e
  `_isatty`/`_fileno` (Windows) para detectar terminal.

**O padrão**: cada projeto isola a portabilidade em funções
pequenas, com `#ifdef`.

## Leitura recomendada

- `console` (Windows):
  <https://learn.microsoft.com/en-us/windows/console/console-functions>
- `termios` (Linux/macOS):
  <https://man7.org/linux/man-pages/man3/termios.3.html>
- ANSI escape code:
  <https://en.wikipedia.org/wiki/ANSI_escape_code>

---

arataca89@gmail.com

Aulas de programação
