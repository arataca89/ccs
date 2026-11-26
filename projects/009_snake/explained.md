# Explicação detalhada do snake

Explicação **linha por linha** do `snake.c`, do `terminal.c` e do
`main.c`, para quem está começando em C e quer entender **o que
cada linha faz** e **por quê**. Não substitui o `notes.md` (que é
sobre decisões e bugs); aqui o foco é **didático**.

## Visão geral do jogo

O Snake é um jogo onde uma cobra se move pelo tabuleiro, come
comida, e cresce. O jogador controla a direção (setas ou WASD).
Bater na parede ou no próprio corpo = game over. Comer o número
alvo de comidas = vitória.

**Estrutura**:

- `terminal.c`: funções de terminal (tamanho, ANSI, teclas,
  sleep). Isola a portabilidade.
- `snake.c`: lógica do jogo (inicialização, atualização,
  renderização).
- `main.c`: parsing de flags, cálculo do tamanho, loop de jogo.

## `terminal.c` — funções de terminal

### `get_terminal_size`

```c
int get_terminal_size(int *cols, int *rows)
{
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
        return -1;
    *cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    *rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    return 0;
#else
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1)
        return -1;
    *cols = w.ws_col;
    *rows = w.ws_row;
    return 0;
#endif
}
```

**O que faz**: obtém o tamanho do terminal (colunas e linhas).

**Windows**: `GetConsoleScreenBufferInfo` retorna o tamanho
**visível** da janela (`srWindow`), não o buffer inteiro (que pode
ser maior por causa do scroll).

**Linux/macOS**: `ioctl(STDOUT_FILENO, TIOCGWINSZ, &w)` retorna as
dimensões do terminal (`w.ws_col`, `w.ws_row`).

**Retorno**: 0 em sucesso, -1 se não for um terminal (ex.:
`stdout` redirecionado).

### `enable_ansi`

```c
void enable_ansi(void)
{
#ifdef _WIN32
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (GetConsoleMode(h, &mode)) {
        mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(h, mode);
    }
#endif
}
```

**O que faz**: habilita o modo ANSI (Virtual Terminal) no Windows.

**Por que**: no Windows 10+, o console **não** interpreta códigos
ANSI (`\033[92m`) por padrão. O `SetConsoleMode` com
`ENABLE_VIRTUAL_TERMINAL_PROCESSING` liga o modo VT.

**No Linux/macOS**: é uma função vazia (o terminal já interpreta
ANSI).

### `read_key` (Windows)

```c
int read_key(void)
{
    if (!_kbhit())
        return -1;
    int c = _getch();
    if (c == 0 || c == 0xE0) {
        int code = _getch();
        switch (code) {
        case 72: return KEY_UP;
        case 80: return KEY_DOWN;
        case 75: return KEY_LEFT;
        case 77: return KEY_RIGHT;
        default: return -1;
        }
    }
    return c;
}
```

**O que faz**: lê uma tecla **sem bloquear**.

**`_kbhit`**: verifica se há tecla **sem bloquear**. Se não há,
retorna -1.

**`_getch`**: lê a tecla **sem eco**.

**Teclas especiais**: as setas (e F1-F12) enviam **2 bytes** (`0`
ou `0xE0`, seguido do código). O `read_key` lê o primeiro byte; se
for `0` ou `0xE0`, lê o segundo e mapeia para `KEY_UP`, etc. (os
valores 72, 80, 75, 77 são os códigos das setas no Windows).

### `read_key` (Linux/macOS)

```c
int read_key(void)
{
    setup_terminal();
    unsigned char c;
    if (read(STDIN_FILENO, &c, 1) != 1)
        return -1;
    if (c == 27) {   /* ESC */
        unsigned char seq[2];
        if (read(STDIN_FILENO, &seq[0], 1) != 1) return -1;
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return -1;
        if (seq[0] == '[') {
            switch (seq[1]) {
            case 'A': return KEY_UP;
            case 'B': return KEY_DOWN;
            case 'C': return KEY_RIGHT;
            case 'D': return KEY_LEFT;
            }
        }
        return -1;
    }
    return c;
}
```

**O que faz**: lê uma tecla **sem bloquear** (modo não-canônico).

**`setup_terminal`**: configura o terminal para **modo
não-canônico** (desliga `ICANON` e `ECHO`, e usa `VMIN = 0` para
leitura não-bloqueante). Ver a seção "Configuração do terminal"
abaixo.

**Teclas especiais**: as setas enviam **3 bytes** (`ESC`, `[`, e
uma letra: `A` = cima, `B` = baixo, `C` = direita, `D` =
esquerda). O `read_key` lê o `ESC`; se for `ESC`, lê os dois bytes
seguintes e mapeia para `KEY_*`.

### `setup_terminal` (Linux/macOS)

```c
static void setup_terminal(void)
{
    if (termios_saved)
        return;
    tcgetattr(STDIN_FILENO, &old_termios);
    struct termios new_termios = old_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO);
    new_termios.c_cc[VMIN] = 0;
    new_termios.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
    termios_saved = 1;
}
```

**O que faz**: configura o terminal para **modo não-canônico** (o
que o Snake precisa).

**`tcgetattr`**: obtém o estado atual do terminal (salva em
`old_termios`, para restaurar depois).

**`ICANON`**: modo **canônico**. No modo canônico, o terminal só
entrega a linha **depois** do Enter, e faz **eco** dos caracteres.
O Snake precisa do **oposto**: entrega imediata, sem eco.

**`ECHO`**: eco dos caracteres. Desligar (o Snake desenha o
tabuleiro, e o eco "sujaria" a tela).

**`VMIN = 0`**: leitura **não-bloqueante**. O `read` retorna
imediatamente, mesmo sem tecla.

**`VTIME = 0`**: sem timeout (retorna imediatamente).

**`tcsetattr`**: aplica o novo estado.

### `sleep_ms`

```c
void sleep_ms(int ms)
{
#ifdef _WIN32
    Sleep((DWORD)ms);
#else
    usleep((useconds_t)ms * 1000);
#endif
}
```

**O que faz**: dorme por `ms` milissegundos.

**Windows**: `Sleep` (em milissegundos).

**Linux/macOS**: `usleep` (em **microssegundos**). O `* 1000`
converte ms para µs.

**Nota**: `usleep` é POSIX, não C padrão. Com C11 estrito, não
compila no Linux (o `useconds_t` não é declarado). Solução:
`CMAKE_C_STANDARD 99` + `CMAKE_C_EXTENSIONS ON` (ver `notes.md`).

### `restore_terminal`

```c
void restore_terminal(void)
{
#ifndef _WIN32
    if (termios_saved) {
        tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
        termios_saved = 0;
    }
#endif
}
```

**O que faz**: restaura o terminal ao estado original (Linux/macOS).

**Por que**: o `setup_terminal` desligou `ICANON` e `ECHO`. Se o
programa terminar sem restaurar, o terminal fica em modo
não-canônico (sem eco, sem Enter), e o usuário precisa reiniciar o
terminal.

**No Windows**: no-op (o `_getch` não muda o terminal
permanentemente).

## `snake.c` — lógica do jogo

### A fila circular

A cobra é uma **fila circular**: o `body` é um array de
`capacity` pontos, e `head`/`tail` são índices que "dão a volta"
(com `% capacity`).

**Visualização** (capacity = 5, length = 3, tail = 3):

```
body: [X][X][ ][A][B]   (X = lixo, A = cauda, B = meio)
                  ^
                  tail = 3

Com length = 3, os elementos são: body[3] (A), body[4] (B), body[0] (C).
O terceiro elemento "deu a volta" (5 % 5 = 0).
```

**Por que fila circular**: a cobra cresce e encolhe. Se fosse um
array linear, precisaria "empurrar" os elementos (O(n)). Com a
fila circular, adicionar na cabeça e remover da cauda é **O(1)**.

### `is_body`: a linha do `idx`

```c
static int is_body(const Game *game, int x, int y)
{
    for (int i = 0; i < game->snake.length; i++) {
        int idx = (game->snake.tail + i) % game->snake.capacity;
        Point p = game->snake.body[idx];
        if (p.x == x && p.y == y)
            return 1;
    }
    return 0;
}
```

**O que faz**: verifica se `(x, y)` é parte do corpo da cobra.

**A linha do `idx`**:

```c
int idx = (game->snake.tail + i) % game->snake.capacity;
```

**O que faz**: calcula o índice real no array `body` do i-ésimo
elemento da cobra, **começando da cauda**.

**Por que precisa do `% capacity`**: porque a cobra é uma **fila
circular**. O `tail` pode ser, por exemplo, 18, e o `length` pode
ser 5. Os elementos estão em 18, 19, 0, 1, 2 (o `% capacity` faz
o índice "dar a volta").

**Exemplo concreto** (capacity = 5, tail = 3, length = 3):

| `i` | `tail + i` | `(tail + i) % capacity` | Posição real |
|---|---|---|---|
| 0 | 3 | 3 | `body[3]` (cauda) |
| 1 | 4 | 4 | `body[4]` |
| 2 | 5 | **0** | `body[0]` (deu a volta!) |

**Sem o `%`**, o `i = 2` acessaria `body[5]` (fora do array). Com
o `%`, acessa `body[0]` (correto).

### `spawn_food`

```c
static void spawn_food(Game *game)
{
    for (;;) {
        int x = 2 + rand() % (game->size - 4);
        int y = 2 + rand() % (game->size - 4);
        if (!is_body(game, x, y) && !(game->food.x == x && game->food.y == y)) {
            game->food.x = x;
            game->food.y = y;
            return;
        }
    }
}
```

**O que faz**: gera uma nova comida em posição aleatória livre
(com **margem de 2** células das bordas).

**`2 + rand() % (size - 4)`**: gera uma posição entre **2** e
`size - 3` (com margem de 2 das bordas). **Por que margem de 2**:
para a comida não ficar **colada** na borda (o que dificultaria a
cobra de comê-la sem bater na parede).

**`for (;;)`**: loop infinito até achar uma posição livre. Se a
comida cair em cima da cobra, tenta de novo.

**Nota**: o `spawn_food` **não** verifica se o tabuleiro está
cheio, porque o `main.c` **valida** o alvo (o alvo não pode ser
maior que o tabuleiro). Então, o tabuleiro **nunca** fica cheio
antes do jogador vencer.

### `game_init`

```c
int game_init(Game *game, int size, int delay, int target)
{
    game->size = size;
    game->delay = delay;
    game->target = target;
    game->score = 0;
    game->game_over = 0;
    game->win = 0;
    game->snake.capacity = target + 1;
    game->snake.body = malloc((size_t)game->snake.capacity * sizeof(Point));
    if (!game->snake.body)
        return -1;
    ...
}
```

**O que faz**: inicializa o jogo (aloca o `body`, posiciona a
cobra no centro, gera a primeira comida).

**`game->snake.capacity = target + 1`**: o `body` tem capacidade
para **`target + 1` segmentos** (1 cabeça + `target` comidas).
**Por que `target + 1`** (e não `size * size`): a cobra **nunca**
passa de `target + 1` segmentos (quando atinge `target` comidas, o
jogo termina). Alocar `size * size` seria **desperdício** de
memória (especialmente em tabuleiros grandes).

**`malloc`**: aloca o `body` dinamicamente. **Sempre** verifique
`NULL`.

**`game->snake.length = 1`**: a cobra começa com **1** segmento
(a cabeça).

**`game->snake.dir = DIR_RIGHT`**: a cobra começa indo para a
**direita**.

### `game_update`

```c
void game_update(Game *game, Direction input)
{
    if (game->game_over)
        return;
    Snake *s = &game->snake;
    if (input != s->dir && !is_opposite(input, s->dir))
        s->dir = input;
    Point new_head = s->body[s->head];
    switch (s->dir) {
    case DIR_UP:    new_head.y--; break;
    case DIR_DOWN:  new_head.y++; break;
    case DIR_LEFT:  new_head.x--; break;
    case DIR_RIGHT: new_head.x++; break;
    }
    ...
}
```

**O que faz**: atualiza o estado (move a cobra, verifica
colisões, come).

**`is_opposite`**: se o jogador aperta "cima" quando a cobra está
indo "baixo", a cobra "entraria" nela mesma. O `is_opposite`
evita isso.

**`Point new_head = s->body[s->head]`**: a nova cabeça é uma
**cópia** da cabeça atual, e depois é **movida** (com `y--`,
`y++`, `x--`, `x++`).

**Colisão com a parede**: se `new_head` está fora do tabuleiro
(`!inside_board`), game over.

**Colisão com o corpo**: percorre de `tail` a `tail + length - 2`
(excluindo a cauda, que será removida se a cobra não comer). Se a
nova cabeça bate no corpo, game over.

**Comeu**: se `new_head` é a comida, `ate = 1`, e o score
incrementa.

**Remove a cauda** (se não comeu): `tail = (tail + 1) % capacity`
(avança a cauda).

**Adiciona a nova cabeça**: `head = (head + 1) % capacity`
(avança a cabeça), e `body[head] = new_head`.

**Se comeu**: verifica se o `score` atingiu o alvo (vitória), e
gera nova comida.

**A verificação de vitória** (pelo `score`):

```c
if (game->score >= game->target) {
    game->win = 1;
    game->game_over = 1;
}
```

**Por que `score` e não `length`**: o `score` é o número de
comidas (o que o jogador vê), e o `length` é o tamanho da cobra
(que inclui a cabeça). A verificação pelo `score` é mais
**intuitiva** para o jogador (o alvo é o número de comidas, não o
tamanho da cobra).

### `game_render`

```c
void game_render(const Game *game)
{
    printf("\033[2J\033[H");
    printf(COLOR_HEAD "CCS SNAKE" COLOR_RESET "\n");
    printf(COLOR_SCORE "Tabuleiro: %dx%d | Alvo: %d | Delay: %d ms"
           COLOR_RESET "\n",
           game->size, game->size, game->target, game->delay);
    for (int y = 0; y < game->size; y++) {
        for (int x = 0; x < game->size; x++) {
            if (x == 0 || x == (game->size - 1) ||
                y == 0 || y == game->size - 1) {
                printf(COLOR_WALL "%c%c" COLOR_RESET, CHAR_WALL, CHAR_WALL);
            } else if (is_head(game, x, y)) {
                printf(COLOR_HEAD "%c " COLOR_RESET, CHAR_HEAD);
            } else if (is_body(game, x, y)) {
                printf(COLOR_BODY "%c " COLOR_RESET, CHAR_BODY);
            } else if (is_food(game, x, y)) {
                printf(COLOR_FOOD "%c " COLOR_RESET, CHAR_FOOD);
            } else {
                printf("  ");
            }
        }
        putchar('\n');
    }
    printf(COLOR_SCORE "Score: %d / %d" COLOR_RESET "\n",
           game->score, game->target);
    ...
}
```

**O que faz**: desenha o tabuleiro.

**`\033[2J\033[H`**: limpa a tela e move o cursor para o topo
(ANSI).

**Título** ("CCS SNAKE"): verde brilhante.

**Parâmetros** (tamanho, **alvo**, delay): amarelo. O alvo é
`target` (não `target - 1`), porque o alvo é o número de comidas.
**Serve para debug** (o jogador vê o que está acontecendo).

**Tabuleiro**:
- **Bordas** (`#`): cinza. **2 caracteres iguais** (borda
  sólida).
- **Cabeça** (`@`): verde brilhante. **Caractere + espaço** (2
  colunas).
- **Corpo** (`+`): verde.
- **Comida** (`*`): vermelho brilhante.
- **Vazio**: **2 espaços**.

**Score** (`Score: X / Y`): amarelo. O `Y` é `target` (o alvo é
o número de comidas; a cobra começa com 1, mas o score é o número
de comidas, não o tamanho da cobra).

**Game over/vitória**: mensagem final, e "Pressione qualquer
tecla".

## `main.c` — parsing e loop

### Parsing de flags

```c
for (int i = 1; i < argc; i++) {
    const char *arg = argv[i];
    if (strcmp(arg, "-h") == 0) {
        print_help(argv[0]);
        return 0;
    }
    else if (strcmp(arg, "-e") == 0) { level = LEVEL_EASY; }
    else if (strcmp(arg, "-m") == 0) { level = LEVEL_MEDIUM; }
    else if (strcmp(arg, "-d") == 0) { level = LEVEL_HARD; }
    else if (strcmp(arg, "-s") == 0) {
        if (++i >= argc || parse_int(argv[i], &size) != 0) {
            fprintf(stderr, "%s: -s requer um numero\n", argv[0]);
            return 1;
        }
        size_set = 1;
    }
    ...
}
```

**O que faz**: percorre `argv`, e para cada flag, atualiza a
variável correspondente.

**`-e`, `-m`, `-d`**: níveis. **`-s`**: tamanho. **`-t`**: alvo.
**`-v`**: delay. **`-h`**: help.

**`++i`**: avança o índice (o próximo argumento é o valor da
flag).

**`size_set`, `delay_set`, `target_set`**: flags booleanas (1 se
o usuário passou a opção, 0 se não). **Por que**: se o usuário
**não** passou `-s`, o `size` é calculado pelo nível; se passou, o
valor do usuário **sobrescreve** o do nível.

### Cálculo do `max_size`

```c
int max_size = (cols / 2 < rows ? cols / 2 : rows) - 6;
if (max_size < MIN_SIZE) {
    fprintf(stderr, "%s: terminal muito pequeno (%dx%d).\n",
            argv[0], cols, rows);
    return 1;
}
```

**O que faz**: calcula o tamanho máximo do tabuleiro (quadrado)
que cabe no terminal.

**`cols / 2`**: cada célula ocupa **2 colunas** (caractere +
espaço).

**`rows`**: cada célula ocupa **1 linha**.

**O menor dos dois**: o tabuleiro precisa caber **na largura e na
altura**, então o limite é o menor.

**`- 6`**: 5 linhas fixas (título, parâmetros, score, mensagem,
"Pressione") + 1 de folga.

**Se `max_size < MIN_SIZE`**: terminal muito pequeno, avisa e sai.

### Aplicação do nível

```c
if (!size_set) {
    int percent;
    switch (level) {
    case LEVEL_EASY:   percent = EASY_PERCENT;   break;
    case LEVEL_MEDIUM: percent = MEDIUM_PERCENT; break;
    case LEVEL_HARD:   percent = HARD_PERCENT;   break;
    }
    size = max_size * percent / 100;
    if (size < MIN_SIZE) size = MIN_SIZE;
    if (size > max_size) size = max_size;
}
```

**O que faz**: se o usuário **não** passou `-s`, calcula o
tamanho do nível como **percentual** do `max_size`.

**`max_size * percent / 100`**: percentual do máximo. Ex.: 70% de
34 = 23.

**Clamp**: se o resultado for menor que `MIN_SIZE` (10), usa 10;
se for maior que `max_size`, usa `max_size`.

**Por que percentual**: o tamanho máximo depende do terminal. Com
percentual, o jogo se adapta a **qualquer** terminal.

**Percentuais atuais** (no `snake.h`):

```c
#define EASY_PERCENT    100
#define MEDIUM_PERCENT  70
#define HARD_PERCENT    50
```

**Por que 100/70/50** (e não 100/50/30): os percentuais
anteriores (50/30) deixavam os níveis médio e difícil **muito
difíceis** (tabuleiro pequeno demais para manobrar). Com 70/50, o
jogo fica mais equilibrado.

### Loop de jogo

```c
while (running && !game.game_over) {
    int key = read_key();
    if (key != -1) {
        switch (key) {
        case 'q': case 'Q': running = 0; break;
        case 'w': case 'W': case KEY_UP:    input = DIR_UP; break;
        ...
        }
    }
    game_update(&game, input);
    game_render(&game);
    sleep_ms(game.delay);
}
```

**O que faz**: o loop principal do jogo.

**`read_key`**: lê uma tecla (não-bloqueante). Se não há tecla,
retorna -1, e o loop continua (a cobra se move sozinha).

**`game_update`**: atualiza o estado (move a cobra, verifica
colisões).

**`game_render`**: desenha o tabuleiro.

**`sleep_ms`**: dorme pelo delay (para o jogo não ser muito
rápido).

**Ordem**: input → update → render → sleep.

### Game over/vitória

```c
if (game.game_over) {
    while (read_key() != -1) ;
    while (read_key() == -1) sleep_ms(50);
    printf("\033[2J\033[H");
    ...
}
```

**O que faz**: se o jogo acabou, espera uma tecla.

**`while (read_key() != -1) ;`**: limpa o buffer de input (teclas
que o usuário apertou durante o jogo).

**`while (read_key() == -1) sleep_ms(50)`**: espera uma tecla
(com delay de 50 ms para não consumir 100% da CPU).

**`\033[2J\033[H`**: limpa a tela e move o cursor para o topo.

**Mensagem final**: "VOCE VENCEU!" ou "GAME OVER!", com o score
final.

### `restore_terminal` e `game_free`

```c
restore_terminal();
game_free(&game);
return 0;
```

**`restore_terminal`**: restaura o terminal (Linux/macOS).

**`game_free`**: libera o `body` (malloc).

**`return 0`**: sucesso.

---

arataca89@gmail.com

Aulas de programação
