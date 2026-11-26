# 009_snake: jogo da cobrinha em terminal

> Implementação didática do clássico jogo Snake, em ASCII, com
> cores ANSI, níveis de dificuldade, e portabilidade Windows/Linux.

## O que faz

O jogador controla uma cobra (`@` + `+`) que se move pelo
tabuleiro. A cobra come comida (`*`) e cresce. Bater na parede
(`#`) ou no próprio corpo = game over. Comer o número alvo de
comidas = vitória.

```
CCS SNAKE
Tabuleiro: 50x50 | Alvo: 19 | Delay: 500 ms
##################################################
#                                                #
#                                                #
#                          @                     #
#                          +                     #
#                          +                     #
#                                                #
#                    *                           #
#                                                #
##################################################
Score: 2 / 19
```

**Controles**: setas ou WASD para mudar a direção. `Q` para sair.

## Onde isso é usado na vida real

- **Jogos de terminal**: o Snake é um clássico dos anos 1970/1980
  (Nokia, MS-DOS, etc.).
- **Conceitos de jogos**: loop de jogo (input → update → render),
  detecção de colisão, estado do jogo.
- **Fila circular**: o corpo da cobra é uma fila circular (O(1)
  para inserir/remover).
- **Terminal**: ANSI escape codes, input não-bloqueante,
  portabilidade Windows/Linux.

## Conceitos de C abordados

- **Terminal**: ANSI escape codes (cores, limpar tela, mover
  cursor), `conio.h`/`termios`.
- **Input não-bloqueante**: `_kbhit`/`_getch` (Windows),
  `read` + `VMIN = 0` (Linux/macOS).
- **Fila circular**: `head`/`tail` com `% capacity` (O(1) para
  inserir/remover).
- **`malloc`/`free`**: alocação dinâmica do `body` da cobra.
- **`enum`**: `Level`, `Direction`.
- **`struct`**: `Point`, `Snake`, `Game`.
- **`#define`**: constantes (tamanhos, delays, alvos, cores,
  caracteres).
- **Aleatoriedade**: `rand`/`srand`.
- **Tempo**: `Sleep` (Windows), `usleep` (Linux/macOS).
- **Portabilidade**: `#ifdef _WIN32` (isolado no `terminal.c`).

## Como compilar

Requer CMake ≥ 3.16 e um compilador C11 (ou C99 + extensions,
por causa do `usleep`).

**Linux/macOS**:

    cmake -S . -B build
    cmake --build build

**Windows com MSYS2 UCRT64 + Ninja**:

    cmake -G Ninja -S . -B build
    cmake --build build

**Nota**: o `009_snake` usa `CMAKE_C_STANDARD 99` +
`CMAKE_C_EXTENSIONS ON` (por causa do `usleep`, que é POSIX, não
C padrão). Ver [notes.md](notes.md) para a justificativa.

## Como executar

**Da raiz do projeto** (`009_snake/`):

    build\snake.exe         (Windows)
    ./build/snake           (Linux/macOS)

**Com níveis**:

    build\snake.exe -e      (fácil: 100% do tabuleiro, 500 ms, alvo 20)
    build\snake.exe -m      (médio: 50% do tabuleiro, 300 ms, alvo 40)
    build\snake.exe -d      (difícil: 30% do tabuleiro, 150 ms, alvo 60)

**Com opções**:

    build\snake.exe -s 30   (tabuleiro 30x30)
    build\snake.exe -t 50   (alvo 50)
    build\snake.exe -v 200  (delay 200 ms, mais rápido que 500)

**Combinando**:

    build\snake.exe -m -t 60     (médio, mas alvo 60)
    build\snake.exe -e -s 30     (fácil, mas tabuleiro 30x30)

## Níveis

Os níveis são **percentuais do tamanho máximo** do terminal:

| Nível | Flag | Tabuleiro | Delay | Alvo |
|---|---|---|---|---|
| **Fácil** (padrão) | `-e` | 100% do máximo | 500 ms | 20 |
| **Médio** | `-m` | 70% do máximo | 300 ms | 40 |
| **Difícil** | `-d` | 50% do máximo | 150 ms | 60 |

**Por que percentual**: o tamanho máximo depende do terminal. Em um
terminal grande, 100% é 50×50; em um pequeno, 100% é 20×20. O jogo
se adapta a qualquer terminal.

**O tamanho máximo** é calculado como
`min(cols / 2, rows) - 6` (5 linhas fixas + 1 de folga).

## Opções

| Opção | O que faz |
|---|---|
| `-e` | Nível fácil (padrão) |
| `-m` | Nível médio |
| `-d` | Nível difícil |
| `-s N` | Tamanho do tabuleiro (limitado ao máximo do terminal) |
| `-t N` | Número alvo de comidas (para vencer) |
| `-v N` | Delay entre frames, em ms (menor = mais rápido) |
| `-h` | Mostra a ajuda e sai |

**As opções `-s`, `-t`, `-v` sobrescrevem os valores do nível.**

## Controles

| Tecla | O que faz |
|---|---|
| Setas | Mudar direção |
| `W`/`A`/`S`/`D` | Mudar direção |
| `Q` | Sair |

## Código de retorno

| Código | Significado |
|---|---|
| 0 | Sucesso (jogo normal ou vitória) |
| 1 | Erro (terminal muito pequeno, argumento inválido, erro de alocação) |

## Nota sobre o tamanho do terminal

O jogo calcula o tamanho máximo do tabuleiro com base no número de
linhas e colunas do terminal. Se o terminal for **muito pequeno**
(menos que `MIN_SIZE + 6` linhas), o jogo **não inicia** e avisa.

**Se o tabuleiro "sumir"** (o título desaparece), diminua o tamanho
do tabuleiro (com `-s`) ou redimensione o terminal.

**A fórmula**: `max_size = min(cols / 2, rows) - 6`. O `- 6` são as
linhas fixas (título, parâmetros, score, mensagem final,
"Pressione", folga).

## Nota sobre as cores

As cores são feitas com **ANSI escape codes** (`\033[92m` etc.). No
Linux/macOS, funciona nativamente. No Windows 10+, o `enable_ansi`
habilita o modo VT.

**Cores**:

| Elemento | Cor |
|---|---|
| Cabeça (`@`) | Verde brilhante |
| Corpo (`+`) | Verde |
| Comida (`*`) | Vermelho brilhante |
| Borda (`#`) | Cinza |
| Pontuação | Amarelo |
| Game over | Vermelho brilhante |

**Nota**: no Linux, as cores brilhantes podem parecer diferentes
(depende do tema do terminal). Se quiser consistência, troque as
cores no `snake.h`.

## Nota sobre o `usleep` (Linux/macOS)

O `usleep` é POSIX, não C padrão. Com C11 estrito
(`CMAKE_C_EXTENSIONS OFF`), ele não compila no Linux. O
`CMakeLists.txt` do `009_snake` usa `CMAKE_C_STANDARD 99` +
`CMAKE_C_EXTENSIONS ON` para habilitar o `usleep`.

**Isso é uma inconsistência** com os outros projetos do CCS (que
usam C11 estrito). Ver [notes.md](notes.md) para a justificativa e
as alternativas (`nanosleep`, `#define _DEFAULT_SOURCE`).

## Testes

Ver [tests/README.md](tests/README.md). Os testes são **comandos de
linha** (não há fixtures de arquivo tradicionais), com gabaritos
medidos.

## Notas de implementação

Ver [notes.md](notes.md). Cobre as decisões (tabuleiro quadrado com
2 caracteres por célula, cores ANSI, fila circular, níveis
percentuais), os bugs (comida colada na borda, título sumindo,
setas não funcionando no Windows/Linux, `usleep` no Linux), e as
lições.

## Explicação detalhada

Ver [explained.md](explained.md). Explicação **linha por linha** do
`snake.c`, do `terminal.c` e do `main.c`, com o **porquê** de cada
decisão (incluindo a fila circular e a leitura não-bloqueante).

## Conceitos relacionados

- [Fila circular](../../docs/circular-queue.md) — o conceito, com
  exemplos e a ligação com o `snake.c`.
- [Input não-bloqueante](../../docs/non-blocking-input.md) — o que
  é, por que o Snake precisa, `_kbhit`/`_getch` no Windows,
  `VMIN = 0`/`read` no Linux/macOS.
- [Cores ANSI](../../docs/ansi-colors.md) — os códigos de escape
  das cores.
- [`#define`, `enum` e `struct`](../../docs/define-enum-struct.md)
  — como o Snake usa cada um.
- [`malloc`](../../docs/malloc-when.md) — quando usar `malloc`.
- [Funções de terminal](../../docs/terminal-functions.md) — as
  funções dependentes de plataforma (`get_terminal_size`,
  `enable_ansi`, `read_key`, `sleep_ms`, `restore_terminal`).

## Referências

- Snake na Wikipedia: <https://en.wikipedia.org/wiki/Snake_(video_game_genre)>
- ANSI escape codes:
  <https://en.wikipedia.org/wiki/ANSI_escape_code>
- `termios` (Linux/macOS):
  <https://man7.org/linux/man-pages/man3/termios.3.html>
- `console` (Windows):
  <https://learn.microsoft.com/en-us/windows/console/console-functions>

---

arataca89@gmail.com

Aulas de programação
