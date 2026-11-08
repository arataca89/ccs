#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

#ifdef _WIN32
#include <io.h>
#define isatty _isatty
#define fileno _fileno
#else
#include <unistd.h>
#endif

#define LINE_MAX 256

static void print_help(void)
{
    printf(
        "Uso: list < comandos.txt\n"
        "     list  (modo interativo)\n"
        "\n"
        "Le comandos de stdin, um por linha. A lista persiste durante\n"
        "a sessao. Comandos disponiveis:\n"
        "\n"
        "  push N      insere N no inicio da lista\n"
        "  append N    insere N no fim da lista\n"
        "  remove N    remove o primeiro no com valor N\n"
        "  find N      busca N (imprime 'encontrado' ou 'nao encontrado')\n"
        "  print       imprime a lista (um valor por linha)\n"
        "  size        imprime o numero de nos\n"
        "  free        libera a lista (e recria uma vazia)\n"
        "  help        mostra esta ajuda\n"
        "  quit        sai do programa\n"
        "\n"
        "Linhas em branco e linhas comecando com '#' sao ignoradas.\n"
        "\n"
        "No modo interativo (terminal), o prompt '> ' aparece antes\n"
        "de cada comando. No modo batch (arquivo redirecionado), o\n"
        "prompt NAO aparece, para a saida ficar limpa.\n"
        "\n"
        "Exemplos:\n"
        "  list < testes/comandos.txt\n"
        "  echo -e 'push 10\\npush 20\\nprint' | list\n"
    );
}

static void strip_newline(char *s)
{
    size_t len = strlen(s);
    while (len > 0 && (s[len-1] == '\n' || s[len-1] == '\r')) {
        s[--len] = '\0';
    }
}

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

int main(void)
{
    List *list = list_create();
    if (!list) {
        fprintf(stderr, "list: erro ao criar a lista\n");
        return 1;
    }

    int interactive = isatty(fileno(stdin));

    char line[LINE_MAX];

    while (1) {
        if (interactive) {
            printf("> ");
            fflush(stdout);
        }

        if (fgets(line, sizeof line, stdin) == NULL)
            break;

        strip_newline(line);

        if (line[0] == '\0' || line[0] == '#')
            continue;

        char *cmd = line;
        char *arg = NULL;

        char *space = strchr(line, ' ');
        if (space) {
            *space = '\0';
            arg = space + 1;
            while (*arg == ' ')
                arg++;
        }

        if (strcmp(cmd, "push") == 0) {
            int v;
            if (!arg || parse_int(arg, &v) != 0) {
                fprintf(stderr, "list: push precisa de um inteiro\n");
                continue;
            }
            if (list_push(list, v) != 0)
                fprintf(stderr, "list: erro de alocacao\n");
        }
        else if (strcmp(cmd, "append") == 0) {
            int v;
            if (!arg || parse_int(arg, &v) != 0) {
                fprintf(stderr, "list: append precisa de um inteiro\n");
                continue;
            }
            if (list_append(list, v) != 0)
                fprintf(stderr, "list: erro de alocacao\n");
        }
        else if (strcmp(cmd, "remove") == 0) {
            int v;
            if (!arg || parse_int(arg, &v) != 0) {
                fprintf(stderr, "list: remove precisa de um inteiro\n");
                continue;
            }
            if (list_remove(list, v))
                printf("removido: %d\n", v);
            else
                printf("nao encontrado: %d\n", v);
        }
        else if (strcmp(cmd, "find") == 0) {
            int v;
            if (!arg || parse_int(arg, &v) != 0) {
                fprintf(stderr, "list: find precisa de um inteiro\n");
                continue;
            }
            if (list_find(list, v))
                printf("encontrado: %d\n", v);
            else
                printf("nao encontrado: %d\n", v);
        }
        else if (strcmp(cmd, "print") == 0) {
            list_print(list);
        }
        else if (strcmp(cmd, "size") == 0) {
            printf("%zu\n", list_size(list));
        }
        else if (strcmp(cmd, "free") == 0) {
            list_free(list);
            list = list_create();
            if (!list) {
                fprintf(stderr, "list: erro ao recriar a lista\n");
                return 1;
            }
            printf("lista liberada\n");
        }
        else if (strcmp(cmd, "help") == 0) {
            print_help();
        }
        else if (strcmp(cmd, "quit") == 0) {
            break;
        }
        else {
            fprintf(stderr, "list: comando desconhecido: '%s'\n", cmd);
            fprintf(stderr, "Tente 'help' para ajuda.\n");
        }
    }

    list_free(list);
    return 0;
}