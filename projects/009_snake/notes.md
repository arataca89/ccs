# Notas de implementação — 009_snake

Registro do raciocínio por trás do código. Não é tutorial: é o
"porquê" das decisões, incluindo os tropeços.

## Por que Snake?

O Snake é um jogo clássico, e cobre conceitos que os projetos
anteriores não cobriram:

1. **Terminal** (ANSI escape codes, `conio.h`/`termios`).
2. **Loop de jogo** (input, update, render, sleep).
3. **Estrutura de dados**: fila circular.
4. **Aleatoriedade** (`rand`/`srand`).
5. **Tempo** (`Sleep`/`usleep`).
6. **Portabilidade** (Windows vs. Linux/macOS).

E é **divertido** (todo mundo gosta de games).

## Decisões de arquitetura

### Tabuleiro quadrado com 2 caracteres por célula

O terminal tem células **retangulares** (mais altas que largas).
Um tabuleiro 20×20 **caracteres** parece **retangular** (mais
alto que largo).

**Solução**: imprimir **2 caracteres por célula** (o caractere +
um espaço). Isso dobra a largura visual, compensando a altura.

**Exemplo**:

```c
printf(COLOR_HEAD "%c " COLOR_RESET, CHAR_HEAD);   /* cabeça + espaço */
printf(COLOR_WALL "%c%c" COLOR_RESET, CHAR_WALL, CHAR_WALL);   /* borda: 2 iguais */
```

**Por que a borda usa 2 caracteres iguais** (`##`): a borda fica
**sólida**, e o tabuleiro fica **fechado** visualmente.

### Cores ANSI

As cores são feitas com **ANSI escape codes** (`\033[...m`). No
Linux/macOS, funciona nativamente. No Windows 10+, precisa
habilitar o **modo VT** (Virtual Terminal) com
`SetConsoleMode` (feito no `enable_ansi`).

**Cores escolhidas**:

| Elemento | Cor | Código |
|---|---|---|
| Cabeça | verde brilhante | `\033[92m` |
| Corpo | verde | `\033[32m` |
| Comida | vermelho brilhante | `\033[91m` |
| Borda | cinza | `\033[90m` |
| Pontuação | amarelo | `\033[93m` |
| Game over | vermelho brilhante | `\033[91m` |

**Nota**: no Linux, as cores brilhantes podem parecer diferentes
(depende do tema do terminal). Se quiser consistência, use as
cores normais (`32`, `31`, `33`, `37`, `30`).

### Tabuleiro dinâmico (malloc)

Com tamanho ajustável, o tabuleiro **não pode** ser um array
fixo. O `body` da cobra é alocado com `malloc` (no `game_init`)
e liberado com `free` (no `game_free`).

**Tamanho**: `target + 1` (o número máximo de segmentos que a
cobra pode atingir — 1 cabeça + `target` comidas). **Por que
`target + 1`**: a cobra nunca passa de `target + 1` segmentos
(quando atinge `target` comidas, o jogo termina).

**Nota**: alocar `target + 1` é **muito menor** que alocar
`size * size` (o tabuleiro inteiro). Exemplo: tabuleiro 50×50
(2500 pontos, 20 KB) vs. alvo 20 (21 pontos, 168 bytes). A
economia é enorme.

### Fila circular

A cobra é uma **fila circular**: o `body` é um array de
`capacity` pontos, e `head`/`tail` são índices que "dão a volta"
(com `% capacity`).

**Por que fila circular**: a cobra cresce e encolhe. Se fosse um
array linear, precisaria "empurrar" os elementos (O(n)). Com a
fila circular, adicionar na cabeça e remover da cauda é **O(1)**.

**Visualização** (capacity = 5, length = 3, tail = 3):

```
body: [X][X][ ][A][B]   (X = lixo, A = cauda, B = meio)
                  ^
                  tail = 3

Com length = 3, os elementos são: body[3] (A), body[4] (B), body[0] (C).
O terceiro elemento "deu a volta" (5 % 5 = 0).
```

### Níveis percentuais

Os níveis são **percentuais do tamanho máximo** do terminal, não
valores fixos:

| Nível | % do máximo | Delay | Alvo (comidas) |
|---|---|---|---|
| Fácil | 100% | 500 ms | 20 |
| Médio | 70% | 300 ms | 40 |
| Difícil | 50% | 150 ms | 60 |

**Por que percentual**: o tamanho máximo depende do terminal. Em
um terminal grande, 100×100 cabe; em um pequeno, não. Com
percentuais, o jogo se adapta a **qualquer** terminal.

**Por que 100/70/50** (e não 100/50/30): os percentuais
anteriores (50/30) deixavam os níveis médio e difícil **muito
difíceis** (tabuleiro pequeno demais para manobrar). Com 70/50, o
jogo fica mais equilibrado.

**Cálculo do máximo**:

```c
int max_size = (cols / 2 < rows ? cols / 2 : rows) - 6;
```

**Por que `- 6`**: o jogo usa 5 linhas fixas (título, parâmetros,
score, mensagem final, "Pressione qualquer tecla") + 1 de folga
(para evitar scroll).

**Se `max_size < MIN_SIZE`**: o jogo não inicia (avisa).

### Alvo = número de comidas (não tamanho da cobra)

O alvo (`target`) é o **número de comidas** que o jogador precisa
comer para vencer. **Não** é o tamanho da cobra (a cabeça não
conta como comida).

**Exemplo**: se `target = 20`, o jogador precisa comer **20
comidas** (o score chega a 20). A cobra terá **21 segmentos** (1
cabeça + 20 comidas).

**A verificação de vitória** é pelo `score`:

```c
if (game->score >= game->target) {
    game->win = 1;
    game->game_over = 1;
}
```

**Por que `score` e não `length`**: o `score` é o número de
comidas (o que o jogador vê), e o `length` é o tamanho da cobra
(que inclui a cabeça). A verificação pelo `score` é mais
**intuitiva** para o jogador.

**No `game_render`**: o alvo é mostrado como `target` (não
`target - 1`), e o score como `X / target`.

## Bugs e correções

### Bug 1: O tabuleiro não ficava quadrado

**Sintoma**: o tabuleiro 20×20 parecia **retangular** (mais alto
que largo).

**Causa**: as células do terminal são retangulares (mais altas
que largas). Um tabuleiro 20×20 **caracteres** fica visualmente
retangular.

**Solução**: imprimir **2 caracteres por célula** (o caractere +
um espaço). Isso dobra a largura, compensando a altura.

### Bug 2: A comida colada na borda

**Sintoma**: a comida aparecia nas posições **adjacentes** às
bordas (1 e `size - 2`). Se a cobra estava indo em direção à
borda, podia não conseguir comer sem bater na parede.

**Causa**: o `spawn_food` gerava a comida em
`1 + rand() % (size - 2)`, que inclui as posições adjacentes às
bordas.

**Solução**: gerar a comida em `2 + rand() % (size - 4)`, com
**margem de 2** células das bordas.

### Bug 3: O título sumia

**Sintoma**: o título "CCS SNAKE" aparecia e **sumia** rapidamente.

**Causa**: o tabuleiro (com título, parâmetros, score) ocupava
**mais linhas** que o terminal. O terminal **rolava** (scroll), e
o título saía da tela.

**Solução**: **remover as linhas em branco** entre título,
parâmetros, tabuleiro e score. O conteúdo total ficou **menor**
que o terminal, e o título ficou fixo.

### Bug 4: O `max_size` não contava as linhas fixas

**Sintoma**: mesmo com o `max_size`, o tabuleiro "estourava" o
terminal em alguns casos.

**Causa**: o `max_size` não contava corretamente as linhas fixas
(título, parâmetros, score, mensagem final, "Pressione").

**Solução**: `max_size = min(cols / 2, rows) - 6` (5 linhas
fixas + 1 de folga).

### Bug 5: As setas não funcionavam (Windows)

**Sintoma**: as setas não funcionavam, mas o WASD sim.

**Causa**: no Windows, as setas enviam **2 bytes** (`0` ou
`0xE0`, seguido do código da tecla). O `read_key` lia só o
primeiro byte (0 ou 224), e o segundo ficava "perdido".

**Solução**: o `read_key` (Windows) lê o primeiro byte; se for
`0` ou `0xE0`, lê o **segundo** e mapeia para `KEY_UP`,
`KEY_DOWN`, `KEY_LEFT`, `KEY_RIGHT`.

### Bug 6: As setas não funcionavam (Linux)

**Sintoma**: as setas não funcionavam no Linux, mas o WASD sim.

**Causa**: no Linux, as setas enviam **3 bytes** (`ESC`, `[`, e
uma letra). O `read_key` lia só o `ESC` (27), e o `[` e a letra
ficavam "perdidos".

**Solução**: o `read_key` (Linux/macOS) lê o primeiro byte; se
for `ESC` (27), lê **mais 2** e mapeia para `KEY_*`.

### Bug 7: O `usleep` não compilava no Linux (C11 estrito)

**Sintoma**: no Linux, o `usleep` e `useconds_t` não eram
declarados (com `CMAKE_C_EXTENSIONS OFF`).

**Causa**: `usleep` é POSIX, não C padrão. Com C11 estrito
(`-std=c11` sem `_DEFAULT_SOURCE`), a `glibc` não expõe
`usleep`.

**Solução**: usar `CMAKE_C_STANDARD 99` + `CMAKE_C_EXTENSIONS ON`
no `CMakeLists.txt` do `009_snake`. Isso define
`_DEFAULT_SOURCE`, e o `usleep` volta a funcionar.

**Nota**: isso cria uma **inconsistência** com os outros projetos
do CCS (que usam C11 estrito). O `notes.md` registra. Se um dia o
CCS quiser voltar ao C11 estrito, o `usleep` precisa ser
substituído por `nanosleep` (POSIX.1-2001), ou o `terminal.c`
precisa de `#define _DEFAULT_SOURCE` no topo.

### Bug 8: O alvo aparecia uma unidade a menos

**Sintoma**: o alvo aparecia como `target - 1` (ex.: 19 em vez de
20).

**Causa**: o `game_render` mostrava `target - 1`, assumindo que o
alvo era o **tamanho da cobra** (que inclui a cabeça). Mas o alvo
é o **número de comidas** (que não inclui a cabeça).

**Solução**: mostrar `target` (não `target - 1`). A verificação
de vitória passou a ser pelo `score` (`score >= target`), não pelo
`length` (`length + 1 >= target`).

### Bug 9: O `capacity` do body era `size * size`

**Sintoma**: o `body` alocava memória para o tabuleiro inteiro
(`size * size`), o que era **desperdício** (a cobra nunca ocupa
tudo, porque a vitória é pelo alvo).

**Causa**: o `game_init` alocava `size * size` pontos.

**Solução**: alocar `target + 1` (o número máximo de segmentos
que a cobra pode atingir). Exemplo: tabuleiro 50×50 (2500 pontos,
20 KB) vs. alvo 20 (21 pontos, 168 bytes).

## Lições gerais deste projeto

1. **Terminal tem células retangulares**: 2 caracteres por célula
   para o tabuleiro ficar quadrado.
2. **Cores ANSI**: funcionam no Linux/macOS nativamente; no
   Windows 10+, precisa `enable_ansi` (modo VT).
3. **Scroll do terminal**: o conteúdo precisa **caber** no
   terminal, senão o topo (título) sai da tela. Remover linhas em
   branco resolve.
4. **`max_size`**: `min(cols / 2, rows) - 6` (5 linhas fixas + 1
   de folga). O cálculo é **universal** (funciona em qualquer
   terminal, desde que `get_terminal_size` reporte corretamente).
5. **Teclas especiais**: Windows (2 bytes) e Linux (3 bytes) têm
   formatos diferentes. O `read_key` precisa ler os bytes extras
   e mapear para `KEY_*`.
6. **`usleep` é POSIX**, não C padrão. Com C11 estrito, não
   compila no Linux. Solução: C99 + extensions ON, ou
   `nanosleep`, ou `#define _DEFAULT_SOURCE`.
7. **Fila circular**: `head`/`tail` com `% capacity` para O(1) em
   inserção/remoção.
8. **Níveis percentuais**: o tamanho do tabuleiro é um
   **percentual** do máximo do terminal, não um valor fixo. Isso
   se adapta a qualquer terminal.
9. **Alvo = número de comidas**: a verificação de vitória é pelo
   `score` (não pelo `length`). A cabeça não conta como comida.
10. **`malloc`/`free`** para o tabuleiro dinâmico (o `body` da
    cobra), com `capacity = target + 1` (não `size * size`).
11. **`rand`/`srand`**: `srand(time(NULL))` para a comida
    aparecer em posições diferentes a cada jogo.

---

arataca89@gmail.com

Aulas de programação
