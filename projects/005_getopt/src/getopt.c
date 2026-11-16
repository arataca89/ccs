#include "getopt.h"

#include <stdio.h>
#include <string.h>


/* Variáveis globais (POSIX). */
char *optarg = NULL;
int optind = 1;
int opterr = 1;
int optopt = 0;

/* Estado interno do getopt, persistente entre chamadas.
 * `nextchar` aponta para o próximo caractere dentro de uma flag
 * agrupada (ex.: "-vo" -> depois de 'v', nextchar aponta para 'o').
 * `first_nonopt` e `last_nonopt` são usados para reordenar argv
 * (não implementado nesta versão; ficam para depois). */
static char *nextchar = NULL;

/* Verifica se `c` está em `optstring` e retorna 1 se a flag
 * espera argumento (tem ':' logo depois), 0 se não, -1 se a flag
 * não existe. */
static int flag_expects_arg(const char *optstring, int c)
{
    const char *p = strchr(optstring, c);
    if (!p)
        return -1;
    if (p[1] == ':')
        return 1;
    return 0;
}

int getopt(int argc, char *const argv[], const char *optstring)
{
    optarg = NULL;
    optopt = 0;

    /* Se não há mais caracteres na flag atual, avança para o
     * próximo argumento. */
    if (nextchar == NULL || *nextchar == '\0') {
        /* Se já passou de todos os argumentos, terminou. */
        if (optind >= argc)
            return -1;

        /* Se o próximo argumento não começa com '-', ou é "-"
         * sozinho, ou é "--", para o parsing. */
        if (argv[optind][0] != '-' || argv[optind][1] == '\0')
            return -1;

        if (strcmp(argv[optind], "--") == 0) {
            optind++;
            return -1;
        }

        /* Avança para o próximo caractere da flag. */
        nextchar = argv[optind] + 1;
        optind++;
    }

    /* Pega o próximo caractere da flag. */
    int c = *nextchar++;
    optopt = c;

    /* Verifica se a flag existe e se espera argumento. */
    int expects = flag_expects_arg(optstring, c);

    if (expects < 0) {
        /* Flag inválida. */
        if (opterr)
            fprintf(stderr, "%s: opcao invalida -- '%c'\n",
                    argv[0], c);
        return '?';
    }

    if (expects == 0) {
        /* Flag sem argumento. */
        return c;
    }

    /* Flag com argumento. */
    if (*nextchar != '\0') {
        /* Argumento colado: "-oarquivo" -> optarg = "arquivo". */
        optarg = nextchar;
        nextchar = NULL;
        return c;
    }

    /* Argumento separado: "-o arquivo" -> optarg = argv[optind]. */
    if (optind >= argc) {
        /* Faltou o argumento. */
        if (opterr)
            fprintf(stderr, "%s: opcao requer argumento -- '%c'\n",
                    argv[0], c);
        return '?';
    }

    optarg = argv[optind];
    optind++;
    nextchar = NULL;
    return c;
}