#include "snake.h"

#include <stdio.h>
#include <stdlib.h>

/* Verifica se `(x, y)` está dentro do tabuleiro (sem contar as
 * bordas). */
static int inside_board(const Game *game, int x, int y)
{
    return x > 0 && x < game->size - 1 &&
           y > 0 && y < game->size - 1;
}

/* Verifica se `(x, y)` é a cabeça da cobra. */
static int is_head(const Game *game, int x, int y)
{
    Point p = game->snake.body[game->snake.head];
    return p.x == x && p.y == y;
}

/* Verifica se `(x, y)` é parte do corpo da cobra (incluindo a
 * cabeça). */
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

/* Verifica se `(x, y)` é a comida. */
static int is_food(const Game *game, int x, int y)
{
    return game->food.x == x && game->food.y == y;
}

/* Gera uma nova comida em posição aleatória livre (com margem
 * de 2 células das bordas). */
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

int game_init(Game *game, int size, int delay, int target)
{
    game->size = size;
    game->delay = delay;
    game->target = target;
    game->score = 0;
    game->game_over = 0;
    game->win = 0;

    /* A cobra nunca passa de `target` segmentos (a vitória é
     * verificada quando o comprimento atinge o alvo). Alocamos
     * `target + 1` para ter folga (a cabeça é adicionada antes
     * da verificação de vitória). */
    game->snake.capacity = target + 1;
    game->snake.body = malloc((size_t)game->snake.capacity * sizeof(Point));
    if (!game->snake.body)
        return -1;

    game->snake.head = 0;
    game->snake.tail = 0;
    game->snake.length = 1;
    game->snake.dir = DIR_RIGHT;

    /* Cobra no centro do tabuleiro. */
    game->snake.body[0].x = size / 2;
    game->snake.body[0].y = size / 2;

    /* Comida em posição aleatória. */
    game->food.x = 2;
    game->food.y = 2;
    spawn_food(game);

    return 0;
}

void game_free(Game *game)
{
    free(game->snake.body);
    game->snake.body = NULL;
}

/* Retorna 1 se a direção `a` é oposta à direção `b`. */
static int is_opposite(Direction a, Direction b)
{
    return (a == DIR_UP && b == DIR_DOWN) ||
           (a == DIR_DOWN && b == DIR_UP) ||
           (a == DIR_LEFT && b == DIR_RIGHT) ||
           (a == DIR_RIGHT && b == DIR_LEFT);
}

void game_update(Game *game, Direction input)
{
    if (game->game_over)
        return;

    Snake *s = &game->snake;

    /* Atualiza a direção (se não for oposta à atual). */
    if (input != s->dir && !is_opposite(input, s->dir))
        s->dir = input;

    /* Calcula a nova cabeça. */
    Point new_head = s->body[s->head];
    switch (s->dir) {
    case DIR_UP:    new_head.y--; break;
    case DIR_DOWN:  new_head.y++; break;
    case DIR_LEFT:  new_head.x--; break;
    case DIR_RIGHT: new_head.x++; break;
    }

    /* Verifica colisão com a parede. */
    if (!inside_board(game, new_head.x, new_head.y)) {
        game->game_over = 1;
        return;
    }

    /* Verifica colisão com o corpo (exceto a cauda, que será
     * removida se a cobra não comer). */
    for (int i = 0; i < s->length - 1; i++) {
        int idx = (s->tail + i) % s->capacity;
        if (s->body[idx].x == new_head.x && s->body[idx].y == new_head.y) {
            game->game_over = 1;
            return;
        }
    }

    /* Verifica se comeu. */
    int ate = (new_head.x == game->food.x && new_head.y == game->food.y);

    if (!ate) {
        /* Remove a cauda (a cobra não cresceu). */
        s->tail = (s->tail + 1) % s->capacity;
        s->length--;
    } else {
        game->score++;

        /* Verifica se atingiu o alvo (vitória). */
        if (game->score >= game->target) {
            game->win = 1;
            game->game_over = 1;
            /* Ainda adiciona a nova cabeça (para o render mostrar
             * a cobra no tamanho final). */
        }
    }

    /* Adiciona a nova cabeça. */
    s->head = (s->head + 1) % s->capacity;
    s->body[s->head] = new_head;
    s->length++;

    /* Se comeu, gera nova comida (a menos que tenha vencido). */
    if (ate && !game->win)
        spawn_food(game);
}

void game_render(const Game *game)
{
    /* Limpa a tela e move o cursor para o topo. */
    printf("\033[2J\033[H");

    /* Título do jogo. */
    printf(COLOR_HEAD "CCS SNAKE" COLOR_RESET "\n");

    /* Parâmetros do jogo (tamanho, alvo, delay). */
    printf(COLOR_SCORE "Tabuleiro: %dx%d | Alvo: %d | Delay: %d ms"
           COLOR_RESET "\n",
           game->size, game->size, game->target, game->delay);

    /* Desenha o tabuleiro. Cada célula imprime 2 caracteres, para
     * compensar a altura do caractere (que é ~2x a largura). */
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

    /* Pontuação e progresso. */
    printf(COLOR_SCORE "Score: %d / %d" COLOR_RESET "\n",
           game->score, game->target);

    /* Game over ou vitória. */
    if (game->game_over) {
        if (game->win) {
            printf(COLOR_HEAD "VOCE VENCEU! Score final: %d" COLOR_RESET "\n",
                   game->score);
        } else {
            printf(COLOR_FOOD "GAME OVER! Score final: %d" COLOR_RESET "\n",
                   game->score);
        }
        printf(COLOR_WALL "Pressione qualquer tecla para sair.\n" COLOR_RESET);
    }
}