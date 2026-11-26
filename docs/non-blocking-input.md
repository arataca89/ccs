# Input não-bloqueante

> "Ler uma tecla sem bloquear" é uma expressão que confunde. Este
> texto explica o que é, por que o Snake precisa, e como funciona
> em Windows e Linux/macOS.

## O que é "bloquear"

Em C, quando você lê do teclado (com `getchar`, `scanf`, `read`,
etc.), a função pode se comportar de duas formas:

### 1. Bloqueante (o padrão)

A função **espera** até que o usuário aperte uma tecla. O programa
**para** naquela linha, e só continua depois que a tecla é
pressionada.

```c
int c = getchar();   // o programa PARA aqui até o usuário apertar uma tecla
```

**Problema para o Snake**: se o `read_key` fosse bloqueante, o jogo
**pararia** esperando o usuário apertar uma tecla. A cobra não se
moveria sozinha. O jogo só avançaria quando o usuário apertasse
algo.

### 2. Não-bloqueante

A função **não espera**. Se não há tecla, retorna **imediatamente**
(com um valor que indica "nenhuma tecla"). O programa **continua**
rodando.

```c
int c = read_key();   // retorna IMEDIATAMENTE (com -1 se não há tecla)
```

**Para o Snake**: o `read_key` é não-bloqueante. Se o usuário não
apertou nada, retorna -1, e o loop continua (a cobra se move
sozinha). Se apertou, retorna a tecla, e o loop processa.

## Por que o Snake precisa de input não-bloqueante

O loop do jogo é:

```c
while (running && !game.game_over) {
    int key = read_key();       // não-bloqueante
    if (key != -1) { ... }      // se há tecla, processa
    game_update(&game, input);  // move a cobra (sempre)
    game_render(&game);         // desenha (sempre)
    sleep_ms(game.delay);       // dorme
}
```

**Se o `read_key` fosse bloqueante**:

```c
int key = read_key();   // o programa PARA aqui até o usuário apertar algo
```

O jogo **pararia** esperando o usuário apertar uma tecla. A cobra
**não se moveria** sozinha. O jogo só avançaria quando o usuário
apertasse algo. **Isso não é o Snake** (o Snake é um jogo onde a
cobra se move continuamente).

**Com o `read_key` não-bloqueante**:

```c
int key = read_key();   // retorna imediatamente (-1 se não há tecla)
```

O jogo **continua** (a cobra se move sozinha), e o `read_key` só
**verifica** se o usuário apertou algo (sem esperar).

## Como o `_kbhit` faz isso (Windows)

No Windows, o `_kbhit` (de `<conio.h>`) **verifica** se há tecla no
buffer de entrada, **sem remover** a tecla. Se não há, retorna 0
(falso). Se há, retorna não-zero (verdadeiro).

```c
if (_kbhit()) {         // há tecla?
    int c = _getch();   // lê a tecla (sem eco)
    // processa
}
// se não há tecla, continua o loop
```

**`_kbhit`** é **não-bloqueante**: retorna **imediatamente** (0 ou
não-zero).

**`_getch`** é **bloqueante**: espera a tecla. Mas, como o
`_kbhit` já verificou que há tecla, o `_getch` **não espera** (a
tecla já está lá).

**A combinação** `_kbhit` + `_getch` dá a leitura **não-bloqueante**:

```c
int read_key(void)
{
    if (!_kbhit())
        return -1;          // não há tecla, retorna -1 (não-bloqueante)
    return _getch();        // há tecla, lê (não espera, porque já está lá)
}
```

## Como o `read` não-bloqueante faz isso (Linux/macOS)

No Linux/macOS, o `read` (de `<unistd.h>`) é **bloqueante por
padrão**. Para torná-lo não-bloqueante, o `setup_terminal`
configura o terminal com `VMIN = 0`:

```c
new_termios.c_cc[VMIN] = 0;   // leitura não-bloqueante
```

**`VMIN = 0`**: o `read` retorna **imediatamente**, mesmo sem
tecla. Se não há tecla, retorna 0 (nada lido). Se há, retorna 1
(um byte lido).

```c
int read_key(void)
{
    unsigned char c;
    if (read(STDIN_FILENO, &c, 1) != 1)   // se não leu nada, retorna -1
        return -1;
    return c;
}
```

**`read(...) != 1`**: se o `read` retornou 0 (nada lido, porque não
há tecla), a condição é verdadeira, e o `read_key` retorna -1.

## O modo não-canônico (Linux/macOS)

No Linux/macOS, o terminal tem dois modos:

- **Canônico** (padrão): o terminal só entrega a linha **depois**
  do Enter, e faz **eco** dos caracteres. O `read` **bloqueia**
  até o Enter.
- **Não-canônico**: o terminal entrega cada caractere
  **imediatamente**, sem eco. O `read` **não bloqueia** (com
  `VMIN = 0`).

**O Snake precisa do não-canônico**:

- **Entrega imediata**: o jogador aperta a seta, e a cobra muda de
  direção **imediatamente** (sem esperar o Enter).
- **Sem eco**: o terminal não "sujaria" a tela com os caracteres
  digitados.

**Configuração** (`setup_terminal`):

```c
new_termios.c_lflag &= ~(ICANON | ECHO);   // desliga canônico e eco
new_termios.c_cc[VMIN] = 0;                // leitura não-bloqueante
new_termios.c_cc[VTIME] = 0;
```

**`ICANON`**: modo canônico. Desligar.

**`ECHO`**: eco. Desligar.

**`VMIN = 0`**: leitura não-bloqueante.

**`VTIME = 0`**: sem timeout (retorna imediatamente).

## Teclas especiais: Windows vs. Linux/macOS

As teclas especiais (setas, F1-F12) enviam **sequências** de bytes,
diferentes em cada plataforma:

### Windows

As setas enviam **2 bytes**: `0` (ou `0xE0`) + o código da tecla.

| Tecla | Bytes | Código |
|---|---|---|
| Seta para cima | `0` `72` | 72 |
| Seta para baixo | `0` `80` | 80 |
| Seta para esquerda | `0` `75` | 75 |
| Seta para direita | `0` `77` | 77 |

**O `read_key`** (Windows):

```c
int c = _getch();
if (c == 0 || c == 0xE0) {
    int code = _getch();   // lê o segundo byte
    switch (code) {
    case 72: return KEY_UP;
    ...
    }
}
return c;   // tecla normal (letra, número, etc.)
```

### Linux/macOS

As setas enviam **3 bytes**: `ESC` (27) + `[` + uma letra.

| Tecla | Bytes | Letra |
|---|---|---|
| Seta para cima | `27` `91` `65` | `A` |
| Seta para baixo | `27` `91` `66` | `B` |
| Seta para direita | `27` `91` `67` | `C` |
| Seta para esquerda | `27` `91` `68` | `D` |

**O `read_key`** (Linux/macOS):

```c
unsigned char c;
read(STDIN_FILENO, &c, 1);
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
return c;   // tecla normal
```

## Por que não usar `scanf` ou `getchar`

- **`scanf`**: é **bloqueante** (espera o Enter) e faz **eco**.
  Não serve para o Snake.
- **`getchar`**: é **bloqueante** (espera o Enter, no modo
  canônico) e faz **eco**. Não serve para o Snake.

**As funções não-bloqueantes** são específicas de plataforma:

- **Windows**: `_kbhit` + `_getch` (de `<conio.h>`).
- **Linux/macOS**: `read` + `termios` (de `<unistd.h>` e
  `<termios.h>`).

## Resumo

| Termo | Significado |
|---|---|
| **Bloqueante** | A função **espera** até que algo aconteça (ex.: o usuário apertar uma tecla). O programa **para**. |
| **Não-bloqueante** | A função **não espera**. Se não há nada, retorna **imediatamente** (com um valor que indica "nada"). O programa **continua**. |

**No Snake**: o `read_key` é **não-bloqueante** porque o jogo
precisa continuar rodando (a cobra se move sozinha), mesmo que o
usuário não aperte nada.

**Windows**: `_kbhit` (verifica) + `_getch` (lê).

**Linux/macOS**: `read` com `VMIN = 0` (leitura não-bloqueante).

## Analogia

- **Bloqueante**: você liga para alguém, e fica **esperando** a
  pessoa atender. Você não faz mais nada até ela atender.
- **Não-bloqueante**: você **verifica** se a pessoa está disponível
  (olha o status online). Se não está, você **continua** fazendo
  outras coisas. Se está, você liga.

No Snake, o `read_key` é o "olhar o status" (não-bloqueante): o
jogo continua rodando, e só quando o usuário aperta uma tecla, o
jogo reage.

## Onde isso aparece no CCS

- **`009_snake`**: o `read_key` (no `terminal.c`) é não-bloqueante.
- **Futuro**: jogos, editores de texto, REPLs (Python, SQLite),
  qualquer programa que precise ler input sem parar.

---

arataca89@gmail.com

Aulas de programação
