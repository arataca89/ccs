#ifndef SNAKE_H
#define SNAKE_H

#include <stddef.h>

/* Percentuais do tamanho máximo (por nível). */
#define EASY_PERCENT    100
#define MEDIUM_PERCENT  70
#define HARD_PERCENT    50

/* Delays e alvos (por nível). */
#define EASY_DELAY    500
#define EASY_TARGET   20

#define MEDIUM_DELAY  300
#define MEDIUM_TARGET 40

#define HARD_DELAY    150
#define HARD_TARGET   60

/* Limites. */
#define MIN_SIZE   10
#define MIN_DELAY  50
#define MAX_DELAY  1000
#define MIN_TARGET 5
#define MAX_TARGET 200

/* Caracteres. */
#define CHAR_HEAD   '@'
#define CHAR_BODY   '+'
#define CHAR_FOOD   '*'
#define CHAR_WALL   '#'
#define CHAR_EMPTY  ' '

/* Cores (ANSI). */
#define COLOR_RESET       "\033[0m"    /* reset */
#define COLOR_HEAD        "\033[92m"   /* verde brilhante (cabeça) */
#define COLOR_BODY        "\033[32m"   /* verde (corpo) */
#define COLOR_FOOD        "\033[91m"   /* vermelho brilhante (comida) */
#define COLOR_WALL        "\033[90m"   /* cinza (bordas) */
#define COLOR_SCORE       "\033[93m"   /* amarelo (pontuação) */
#define COLOR_GAMEOVER    "\033[91m"   /* vermelho brilhante (game over) */

/* Níveis de dificuldade. */
typedef enum {
    LEVEL_EASY,
    LEVEL_MEDIUM,
    LEVEL_HARD
} Level;

/* Direções. */
typedef enum {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;

/* Ponto no tabuleiro. */
typedef struct {
    int x;
    int y;
} Point;

/* Cobra: fila circular de pontos. */
typedef struct {
    Point *body;
    int capacity;
    int head;
    int tail;
    int length;
    Direction dir;
} Snake;

/* Estado do jogo. */
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

int game_init(Game *game, int size, int delay, int target);
void game_free(Game *game);
void game_update(Game *game, Direction input);
void game_render(const Game *game);

#endif /* SNAKE_H */