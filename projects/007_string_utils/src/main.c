#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strings.h"

static void print_help(const char *prog)
{
    printf(
        "Uso: %s COMANDO [ARGUMENTOS...]\n"
        "\n"
        "Demonstra as funcoes do string_utils.\n"
        "\n"
        "Comandos:\n"
        "  dup STR           duplica STR\n"
        "  ndup N STR        duplica os primeiros N caracteres de STR\n"
        "  trim STR          remove espacos do inicio e fim de STR\n"
        "  rev STR           inverte STR (in-place)\n"
        "  lower STR         converte STR para minusculas (in-place)\n"
        "  upper STR         converte STR para maiusculas (in-place)\n"
        "  split STR DELIM   divide STR por DELIM (um caractere)\n"
        "  join SEP STR...   junta STR... com SEP\n"
        "  help              mostra esta ajuda\n"
        "\n"
        "Exemplos:\n"
        "  %s dup \"ola\"\n"
        "  %s ndup 3 \"ola mundo\"\n"
        "  %s trim \"  ola  \"\n"
        "  %s rev \"ola\"\n"
        "  %s lower \"OLA\"\n"
        "  %s upper \"ola\"\n"
        "  %s split \"a,b,c\" ,\n"
        "  %s join \", \" a b c\n",
        prog, prog, prog, prog, prog, prog, prog, prog, prog
    );
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        print_help(argv[0]);
        return 1;
    }

    const char *cmd = argv[1];

    /* dup STR */
    if (strcmp(cmd, "dup") == 0) {
        if (argc != 3) {
            fprintf(stderr, "%s: dup requer uma string\n", argv[0]);
            return 1;
        }
        char *r = strdup(argv[2]);
        if (!r) {
            fprintf(stderr, "%s: erro de alocacao\n", argv[0]);
            return 1;
        }
        printf("%s\n", r);
        free(r);
    }

    /* ndup N STR */
    else if (strcmp(cmd, "ndup") == 0) {
        if (argc != 4) {
            fprintf(stderr, "%s: ndup requer N e string\n", argv[0]);
            return 1;
        }
        char *end;
        long n = strtol(argv[2], &end, 10);
        if (*end != '\0' || end == argv[2] || n < 0) {
            fprintf(stderr, "%s: ndup: N invalido\n", argv[0]);
            return 1;
        }
        char *r = strndup(argv[3], (size_t)n);
        if (!r) {
            fprintf(stderr, "%s: erro de alocacao\n", argv[0]);
            return 1;
        }
        printf("%s\n", r);
        free(r);
    }

    /* trim STR */
    else if (strcmp(cmd, "trim") == 0) {
        if (argc != 3) {
            fprintf(stderr, "%s: trim requer uma string\n", argv[0]);
            return 1;
        }
        char *r = strtrim(argv[2]);
        if (!r) {
            fprintf(stderr, "%s: erro de alocacao\n", argv[0]);
            return 1;
        }
        printf("%s\n", r);
        free(r);
    }

    /* rev STR */
    else if (strcmp(cmd, "rev") == 0) {
        if (argc != 3) {
            fprintf(stderr, "%s: rev requer uma string\n", argv[0]);
            return 1;
        }
        /* strrev modifica in-place; precisamos de uma copia
         * mutavel (argv[2] pode ser read-only em alguns casos). */
        char *r = strdup(argv[2]);
        if (!r) {
            fprintf(stderr, "%s: erro de alocacao\n", argv[0]);
            return 1;
        }
        str_reverse(r);
        printf("%s\n", r);
        free(r);
    }

    /* lower STR */
    else if (strcmp(cmd, "lower") == 0) {
        if (argc != 3) {
            fprintf(stderr, "%s: lower requer uma string\n", argv[0]);
            return 1;
        }
        char *r = strdup(argv[2]);
        if (!r) {
            fprintf(stderr, "%s: erro de alocacao\n", argv[0]);
            return 1;
        }
        strtolower(r);
        printf("%s\n", r);
        free(r);
    }

    /* upper STR */
    else if (strcmp(cmd, "upper") == 0) {
        if (argc != 3) {
            fprintf(stderr, "%s: upper requer uma string\n", argv[0]);
            return 1;
        }
        char *r = strdup(argv[2]);
        if (!r) {
            fprintf(stderr, "%s: erro de alocacao\n", argv[0]);
            return 1;
        }
        strtoupper(r);
        printf("%s\n", r);
        free(r);
    }

    /* split STR DELIM */
    else if (strcmp(cmd, "split") == 0) {
        if (argc != 4) {
            fprintf(stderr, "%s: split requer string e delimitador\n",
                    argv[0]);
            return 1;
        }
        char delim = argv[3][0];
        char **tokens = strsplit(argv[2], delim);
        if (!tokens) {
            fprintf(stderr, "%s: erro de alocacao\n", argv[0]);
            return 1;
        }
        for (char **p = tokens; *p; p++)
            printf("%s\n", *p);
        for (char **p = tokens; *p; p++)
            free(*p);
        free(tokens);
    }

    /* join SEP STR... */
    else if (strcmp(cmd, "join") == 0) {
        if (argc < 4) {
            fprintf(stderr, "%s: join requer separador e strings\n",
                    argv[0]);
            return 1;
        }
        const char *sep = argv[2];
        /* argv[3..argc-1] sao as strings; precisamos de um array
         * terminado em NULL. */
        int n = argc - 3;
        char **parts = malloc((size_t)(n + 1) * sizeof(char *));
        if (!parts) {
            fprintf(stderr, "%s: erro de alocacao\n", argv[0]);
            return 1;
        }
        for (int i = 0; i < n; i++)
            parts[i] = argv[3 + i];
        parts[n] = NULL;

        char *r = strjoin(parts, sep);
        free(parts);
        if (!r) {
            fprintf(stderr, "%s: erro de alocacao\n", argv[0]);
            return 1;
        }
        printf("%s\n", r);
        free(r);
    }

    /* help */
    else if (strcmp(cmd, "help") == 0) {
        print_help(argv[0]);
    }

    /* comando desconhecido */
    else {
        fprintf(stderr, "%s: comando desconhecido: '%s'\n",
                argv[0], cmd);
        fprintf(stderr, "Tente '%s help' para ajuda.\n", argv[0]);
        return 1;
    }

    return 0;
}