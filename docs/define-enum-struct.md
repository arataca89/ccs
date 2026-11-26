# `#define`, `enum` e `struct`: conectando a necessidade à ferramenta

> Em C, há três formas de agrupar dados e conceitos: `#define`
> (constantes), `enum` (conjuntos de valores) e `struct` (tipos
> compostos). Este texto mostra **quando** usar cada um, com
> exemplos do `009_snake`.

## O problema: como representar conceitos do software

Quando você projeta um software, aparecem **conceitos** que
precisam ser representados no código:

- **Constantes**: o tabuleiro tem um tamanho máximo? A cor da
  cabeça é fixa?
- **Conjuntos de valores**: a direção é cima, baixo, esquerda,
  direita. O nível é fácil, médio, difícil.
- **Tipos compostos**: um ponto tem `x` e `y`. A cobra tem corpo,
  cabeça, cauda. O jogo tem cobra, comida, score.

**Cada conceito tem uma ferramenta adequada**:

| Conceito | Ferramenta |
|---|---|
| Constante (valor fixo) | `#define` (ou `const`) |
| Conjunto de valores (um de vários) | `enum` |
| Tipo composto (vários campos) | `struct` |

**O problema**: os alunos muitas vezes **não conectam** a
necessidade à ferramenta. Usam `int` para tudo, ou `#define` para
tudo, ou não sabem quando usar `enum`/`struct`.

## `#define`: constantes

**O que é**: uma **macro de pré-processador**. O `#define` substitui
o texto **antes** da compilação.

```c
#define MAX_SIZE 100
```

**O que acontece**: o pré-processador substitui `MAX_SIZE` por
`100` em **todo** o código, antes de o compilador ver.

**Quando usar**:

- **Constantes de compilação**: valores que **não mudam** durante a
  execução (tamanho de array, limites, cores).
- **Macros simples**: funções curtas (mas cuidado com os efeitos
  colaterais).

**Exemplo no `snake.h`**:

```c
#define EASY_PERCENT    100
#define EASY_DELAY      500
#define EASY_TARGET     20

#define MIN_SIZE        10
#define MIN_DELAY       50
#define MAX_DELAY       1000

#define COLOR_HEAD      "\033[92m"
#define CHAR_HEAD       '@'
```

**Por que `#define` e não `const`**:

- **`#define`**: substituição textual, **sem tipo**, **sem
  alocação de memória**. Funciona em **qualquer** contexto
  (inclusive em `#if`, tamanho de array, etc.).
- **`const`**: variável **com tipo**, **com alocação** (a menos que
  o compilador otimize). Não funciona em `#if` nem em tamanho de
  array (em C89; em C99, VLA é diferente).

**No `snake`**: usamos `#define` para as constantes, porque são
**valores fixos** (não mudam), e algumas são usadas em contextos
que exigem constante de compilação.

## `enum`: conjuntos de valores

**O que é**: um **tipo** que define um conjunto de **valores
nomeados**. Cada valor tem um **nome** e um **número** (implícito
ou explícito).

```c
typedef enum {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;
```

**O que acontece**: o compilador cria um tipo `Direction`, e
`DIR_UP` = 0, `DIR_DOWN` = 1, `DIR_LEFT` = 2, `DIR_RIGHT` = 3.

**Quando usar**:

- **Conjuntos de valores** que representam **estados** ou
  **categorias**: direções, níveis, cores, dias da semana.
- Quando o valor **só pode ser um** de um conjunto conhecido.

**Exemplo no `snake.h`**:

```c
typedef enum {
    LEVEL_EASY,
    LEVEL_MEDIUM,
    LEVEL_HARD
} Level;
```

**Por que `enum` e não `#define`**:

- **`#define`**: cada valor é uma **macro separada**, sem relação
  entre si. O compilador não sabe que `DIR_UP` e `DIR_DOWN` são do
  mesmo conjunto.
- **`enum`**: o compilador sabe que `DIR_UP` é do tipo `Direction`,
  e **avisa** se você tentar usar um valor de outro tipo (com
  warnings).

**Exemplo de erro que o `enum` pega**:

```c
Direction d = DIR_UP;
int level = LEVEL_EASY;   // OK (enum é int)
d = level;                // warning (Level não é Direction)
```

**No `snake`**: usamos `enum` para `Direction` e `Level`, porque
são **conjuntos de valores** que representam categorias, e o
compilador pode ajudar a evitar erros.

## `struct`: tipos compostos

**O que é**: um **tipo** que agrupa **vários campos** (de tipos
diferentes) em uma única unidade.

```c
typedef struct {
    int x;
    int y;
} Point;
```

**O que acontece**: o compilador cria um tipo `Point` com dois
campos (`x` e `y`), cada um um `int`.

**Quando usar**:

- **Agrupar dados relacionados**: um ponto (x, y), uma pessoa
  (nome, idade), uma cobra (corpo, cabeça, cauda).
- Quando um conceito tem **vários atributos**.

**Exemplo no `snake.h`**:

```c
typedef struct {
    Point *body;
    int capacity;
    int head;
    int tail;
    int length;
    Direction dir;
} Snake;

typedef struct {
    Snake snake;
    Point food;
    int score;
    int game_over;
    int win;
    int size;
    int delay;
    int target;
} Game;
```

**Por que `struct` e não variáveis soltas**:

- **Agrupamento**: os campos de `Point` (x, y) andam **juntos**.
  Se você tem `int x, y;` soltos, é fácil confundir com outras
  variáveis.
- **Passagem por parâmetro**: você passa `Point p` (ou `Point *p`)
  como **um** parâmetro, não dois.
- **Funções**: as funções recebem `Game *game` (um ponteiro para a
  struct), não 10 parâmetros soltos.

**Exemplo de função com struct**:

```c
void game_update(Game *game, Direction input)
```

**Sem struct**, seria:

```c
void game_update(Snake *snake, Point *food, int *score,
                 int *game_over, int *win, int size,
                 int delay, int target, Direction input)
```

**A struct** deixa a assinatura **muito** mais limpa.

**No `snake`**: usamos `struct` para `Point` (x, y), `Snake`
(corpo, cabeça, cauda, etc.) e `Game` (cobra, comida, score,
etc.), porque são **conceitos com vários atributos**.

## Como escolher

| Necessidade | Ferramenta | Exemplo no Snake |
|---|---|---|
| Valor fixo (constante) | `#define` | `MAX_SIZE`, `COLOR_HEAD`, `CHAR_HEAD` |
| Conjunto de valores (um de vários) | `enum` | `Direction` (cima/baixo/esquerda/direita), `Level` (fácil/médio/difícil) |
| Tipo composto (vários campos) | `struct` | `Point` (x, y), `Snake` (corpo, cabeça, etc.), `Game` (cobra, comida, etc.) |

**Regras práticas**:

1. **Se o valor não muda**, e é uma constante, use `#define` (ou
   `const`).
2. **Se o valor é um de um conjunto conhecido**, use `enum`.
3. **Se o conceito tem vários atributos**, use `struct`.
4. **Se você tem muitos parâmetros relacionados**, agrupe em uma
   `struct` (e passe um ponteiro para a struct).

## Erros comuns

### 1. Usar `#define` para tudo

```c
#define DIR_UP    0
#define DIR_DOWN  1
#define DIR_LEFT  2
#define DIR_RIGHT 3
```

**Problema**: o compilador não sabe que esses valores são do mesmo
conjunto. Você pode passar `DIR_UP` onde espera `Level`, e o
compilador não avisa.

**Solução**: use `enum`.

### 2. Usar `int` para tudo

```c
int direction = 0;   // o que é 0? cima? baixo?
```

**Problema**: o código fica **ilegível** (o que é 0?). E o
compilador não avisa se você passar um valor inválido.

**Solução**: use `enum`.

### 3. Ter 10 parâmetros soltos

```c
void game_update(Snake *s, Point *food, int *score, ...);
```

**Problema**: a assinatura fica **enorme**, e é fácil confundir a
ordem dos parâmetros.

**Solução**: agrupe em uma `struct` (`Game`), e passe um ponteiro.

### 4. `struct` sem `typedef`

```c
struct Point {
    int x;
    int y;
};
struct Point p;   // precisa do "struct" toda vez
```

**Solução**: use `typedef`:

```c
typedef struct {
    int x;
    int y;
} Point;

Point p;   // sem "struct"
```

**No `snake`**: usamos `typedef` em todas as structs.

## No `009_snake`

O `snake.h` usa as três ferramentas:

**`#define`** (constantes):

```c
#define EASY_PERCENT    100
#define EASY_DELAY      500
#define EASY_TARGET     20
#define MIN_SIZE        10
#define COLOR_HEAD      "\033[92m"
#define CHAR_HEAD       '@'
```

**`enum`** (conjuntos de valores):

```c
typedef enum {
    LEVEL_EASY,
    LEVEL_MEDIUM,
    LEVEL_HARD
} Level;

typedef enum {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;
```

**`struct`** (tipos compostos):

```c
typedef struct {
    int x;
    int y;
} Point;

typedef struct {
    Point *body;
    int capacity;
    int head;
    int tail;
    int length;
    Direction dir;
} Snake;

typedef struct {
    Snake snake;
    Point food;
    int score;
    int game_over;
    int win;
    int size;
    int delay;
    int target;
} Game;
```

**A conexão**:

- **`#define`**: os valores **não mudam** (tamanho mínimo, cores,
  caracteres). São constantes de compilação.
- **`enum`**: a **direção** e o **nível** são **um de um conjunto
  conhecido**. O compilador ajuda a evitar erros.
- **`struct`**: o **ponto**, a **cobra** e o **jogo** têm **vários
  atributos**. A `struct` agrupa, e as funções passam **um**
  ponteiro (não 10 parâmetros).

## Resumo

| Ferramenta | O que é | Quando usar | Exemplo no Snake |
|---|---|---|---|
| **`#define`** | Macro (substituição textual) | Constantes de compilação | `MAX_SIZE`, `COLOR_HEAD`, `CHAR_HEAD` |
| **`enum`** | Tipo com valores nomeados | Conjuntos de valores | `Direction`, `Level` |
| **`struct`** | Tipo com campos | Conceitos com vários atributos | `Point`, `Snake`, `Game` |

**Regras**:

1. **Constante** → `#define` (ou `const`).
2. **Conjunto de valores** → `enum`.
3. **Vários atributos** → `struct`.
4. **Muitos parâmetros** → agrupe em `struct`.
5. **`typedef`** em todas as structs e enums (para não precisar do
   `struct`/`enum` toda vez).

## Onde isso aparece no CCS

- **`001_wc`**: `#define` para `BLOCK`; `struct WcCounts` para as
  contagens.
- **`002_cat`**: `#define` para `LINE_MAX`; `struct CatOptions`
  para as opções.
- **`003_grep`**: `struct GrepOptions` para as opções.
- **`004_linked_list`**: `struct Node`, `struct List` para a
  lista; `#define` para `LINE_MAX`.
- **`005_getopt`**: `#define` para as variáveis globais.
- **`006_hexdump`**: `#define` para `BLOCK`; `struct
  HexdumpOptions` para as opções.
- **`007_string_utils`**: `#define` para as cores (não, não tem
  cores); `struct` não tem (só funções).
- **`008_rle`**: `#define` para `MAX_COUNT`.
- **`009_snake`**: `#define` (constantes), `enum` (`Direction`,
  `Level`), `struct` (`Point`, `Snake`, `Game`).

**O padrão é consistente**: cada projeto usa as três ferramentas
conforme a necessidade.

---

arataca89@gmail.com

Aulas de programação
