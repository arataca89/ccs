#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "snake.h"
#include "terminal.h"

static void print_help(const char *prog)
{
    printf(
        "Uso: %s [NIVEL] [OPCOES]\n"
        "     %s -h\n"
        "\n"
        "Jogo Snake em terminal. A cobra (@) come a comida (*) e\n"
        "cresce. Bater na parede (#) ou no proprio corpo = game over.\n"
        "Comer o numero alvo de comidas = vitoria.\n"
        "\n"
        "Niveis (o padrao eh -e):\n"
        "  -e    facil    (100%% do tabuleiro maximo, delay 500 ms, alvo 20)\n"
        "  -m    medio    (50%% do tabuleiro maximo, delay 300 ms, alvo 40)\n"
        "  -d    dificil  (30%% do tabuleiro maximo, delay 150 ms, alvo 60)\n"
        "\n"
        "Opcoes (sobrescrevem os valores do nivel):\n"
        "  -s N  tamanho do tabuleiro (limitado ao maximo do terminal)\n"
        "  -t N  numero alvo de comidas (para vencer)\n"
        "  -v N  delay entre frames, em ms (menor = mais rapido)\n"
        "\n"
        "Controles:\n"
        "  Setas ou WASD   mudar direcao\n"
        "  Q               sair\n"
        "\n"
        "Exemplos:\n"
        "  %s              (nivel facil)\n"
        "  %s -d           (nivel dificil)\n"
        "  %s -m -t 60     (nivel medio, mas alvo 60)\n"
        "  %s -e -s 30     (nivel facil, tabuleiro 30x30)\n",
        prog, prog, prog, prog, prog, prog
    );
}

/* Converte string para int. Retorna 0 em sucesso, -1 em erro. */
static int parse_int(const char *s, int *out)
{
    char *end;
    long v = strtol(s, &end, 10);

    while (*end == ' ' || *end == '\t')
        end++;

    if (*end != '\0' || end == s)
        return -1;

    *out = (int)v;
    return 0;
}

int main(int argc, char **argv)
{
    Level level = LEVEL_EASY;
    int size = 0, delay = 0, target = 0;
    int size_set = 0, delay_set = 0, target_set = 0;

    /* Parsing de flags. */
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
        else if (strcmp(arg, "-t") == 0) {
            if (++i >= argc || parse_int(argv[i], &target) != 0) {
                fprintf(stderr, "%s: -t requer um numero\n", argv[0]);
                return 1;
            }
            target_set = 1;
        }
        else if (strcmp(arg, "-v") == 0) {
            if (++i >= argc || parse_int(argv[i], &delay) != 0) {
                fprintf(stderr, "%s: -v requer um numero\n", argv[0]);
                return 1;
            }
            delay_set = 1;
        }
        else {
            fprintf(stderr, "%s: argumento invalido: '%s'\n", argv[0], arg);
            fprintf(stderr, "Tente '%s -h' para ajuda.\n", argv[0]);
            return 1;
        }
    }

    /* Obtém o tamanho do terminal. */
    int cols, rows;
    if (get_terminal_size(&cols, &rows) != 0) {
        fprintf(stderr, "%s: nao foi possivel obter o tamanho do terminal\n",
                argv[0]);
        fprintf(stderr, "Verifique se voce esta em um terminal interativo.\n");
        return 1;
    }

    /* Calcula o tamanho máximo do tabuleiro (5 linhas fixas + 1
     * de folga = 6). */
    int max_size = (cols / 2 < rows ? cols / 2 : rows) - 6;
    if (max_size < MIN_SIZE) {
        fprintf(stderr, "%s: terminal muito pequeno (%dx%d).\n",
                argv[0], cols, rows);
        fprintf(stderr, "Minimo: %dx%d.\n", MIN_SIZE + 6, MIN_SIZE + 6);
        return 1;
    }

    /* Aplica os valores padrão do nível (se não sobrescritos). */
    if (!size_set) {
        int percent = 0;
        switch (level) {
        case LEVEL_EASY:   percent = EASY_PERCENT;   break;
        case LEVEL_MEDIUM: percent = MEDIUM_PERCENT; break;
        case LEVEL_HARD:   percent = HARD_PERCENT;   break;
        }
        size = max_size * percent / 100;
        if (size < MIN_SIZE) size = MIN_SIZE;
        if (size > max_size) size = max_size;
    }
    if (!delay_set) {
        switch (level) {
        case LEVEL_EASY:   delay = EASY_DELAY;   break;
        case LEVEL_MEDIUM: delay = MEDIUM_DELAY; break;
        case LEVEL_HARD:   delay = HARD_DELAY;   break;
        }
    }
    if (!target_set) {
        switch (level) {
        case LEVEL_EASY:   target = EASY_TARGET;   break;
        case LEVEL_MEDIUM: target = MEDIUM_TARGET; break;
        case LEVEL_HARD:   target = HARD_TARGET;   break;
        }
    }

    /* Valida o tamanho mínimo. */
    if (size < MIN_SIZE) {
        fprintf(stderr, "%s: tamanho minimo: %d\n", argv[0], MIN_SIZE);
        return 1;
    }

    /* Valida o delay. */
    if (delay < MIN_DELAY || delay > MAX_DELAY) {
        fprintf(stderr, "%s: delay deve estar entre %d e %d ms\n",
                argv[0], MIN_DELAY, MAX_DELAY);
        return 1;
    }

    /* Valida o alvo. */
    if (target < MIN_TARGET || target > MAX_TARGET) {
        fprintf(stderr, "%s: alvo deve estar entre %d e %d\n",
                argv[0], MIN_TARGET, MAX_TARGET);
        return 1;
    }

    /* Limita o tamanho ao máximo do terminal (para o -s manual). */
    if (size > max_size) {
        fprintf(stderr, "%s: terminal suporta no maximo %dx%d. Usando %d.\n",
                argv[0], max_size, max_size, max_size);
        size = max_size;
    }

    /* Valida o alvo contra o tabuleiro final. */
    int max_cells = (size - 4) * (size - 4);
    if (target > max_cells) {
        fprintf(stderr,
                "%s: alvo %d e muito grande para tabuleiro %dx%d.\n",
                argv[0], target, size, size);
        fprintf(stderr, "Maximo: %d comidas.\n", max_cells);
        return 1;
    }

    /* Habilita ANSI (Windows). */
    enable_ansi();

    /* Inicializa o jogo. */
    Game game;
    if (game_init(&game, size, delay, target) != 0) {
        fprintf(stderr, "%s: erro de alocacao\n", argv[0]);
        return 1;
    }

    /* Inicializa o gerador de números aleatórios. */
    srand((unsigned)time(NULL));

    /* GAME LOOP */
    Direction input = game.snake.dir;
    int running = 1;

    while (running && !game.game_over) {
        int key = read_key();
        if (key != -1) {
            switch (key) {
            case 'q': case 'Q':
                running = 0;
                break;
            case 'w': case 'W': case KEY_UP:    input = DIR_UP; break;
            case 's': case 'S': case KEY_DOWN:  input = DIR_DOWN; break;
            case 'a': case 'A': case KEY_LEFT:  input = DIR_LEFT; break;
            case 'd': case 'D': case KEY_RIGHT: input = DIR_RIGHT; break;
            }
        }

        game_update(&game, input);
        game_render(&game);
        sleep_ms(game.delay);
    }

    /* Se o jogo acabou (game over ou vitória), espera uma tecla. */
    if (game.game_over) {
        while (read_key() != -1)
            ;
        while (read_key() == -1)
            sleep_ms(50);

        printf("\033[2J\033[H");
        if (game.win) {
            printf(COLOR_HEAD "VOCE VENCEU! Score final: %d" COLOR_RESET "\n",
                   game.score);
        } else {
            printf(COLOR_GAMEOVER "GAME OVER! Score final: %d" COLOR_RESET "\n",
                   game.score);
        }
        printf("Tamanho do tabuleiro: %dx%d\n", game.size, game.size);
        printf("Delay: %d ms\n", game.delay);
        printf("Alvo: %d comidas\n", game.target - 1);
    }

    /* Restaura o terminal (Linux/macOS). */
    restore_terminal();

    /* Libera a memória. */
    game_free(&game);

    return 0;
}