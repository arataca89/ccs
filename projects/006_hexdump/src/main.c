#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hexdump.h"

static void print_help(const char *prog)
{
    printf(
        "Uso: %s [OPCOES] [ARQUIVO]\n"
        "\n"
        "Mostra o conteudo de ARQUIVO em hexadecimal e ASCII.\n"
        "Sem ARQUIVO, le de stdin.\n"
        "\n"
        "Opcoes:\n"
        "  -s N    pula os primeiros N bytes\n"
        "  -n N    le no maximo N bytes\n"
        "  -h      mostra esta ajuda e sai\n"
        "\n"
        "Exemplos:\n"
        "  %s arquivo.bin\n"
        "  %s -s 16 arquivo.bin\n"
        "  %s -n 64 arquivo.bin\n"
        "  type arquivo.bin | %s\n",
        prog, prog, prog, prog, prog
    );
}

/* Converte string para unsigned long. Retorna 0 em sucesso,
 * -1 em erro. */
static int parse_ulong(const char *s, unsigned long *out)
{
    char *end;
    unsigned long v = strtoul(s, &end, 10);

    while (*end == ' ' || *end == '\t')
        end++;

    if (*end != '\0' || end == s)
        return -1;

    *out = v;
    return 0;
}

int main(int argc, char **argv)
{
    HexdumpOptions opts = {0};
    const char *path = NULL;

    /* Parsing manual de flags. */
    for (int i = 1; i < argc; i++) {
        const char *arg = argv[i];

        if (arg[0] != '-' || arg[1] == '\0') {
            /* Não é flag: é o arquivo. */
            path = arg;
            continue;
        }

        /* Flag. */
        switch (arg[1]) {
        case 's':
            if (i + 1 >= argc) {
                fprintf(stderr, "%s: -s requer um numero\n", argv[0]);
                return 1;
            }
            if (parse_ulong(argv[++i], &opts.skip) != 0) {
                fprintf(stderr, "%s: -s: numero invalido\n", argv[0]);
                return 1;
            }
            break;

        case 'n':
            if (i + 1 >= argc) {
                fprintf(stderr, "%s: -n requer um numero\n", argv[0]);
                return 1;
            }
            if (parse_ulong(argv[++i], &opts.limit) != 0) {
                fprintf(stderr, "%s: -n: numero invalido\n", argv[0]);
                return 1;
            }
            break;

        case 'h':
            print_help(argv[0]);
            return 0;

        default:
            fprintf(stderr, "%s: opcao invalida -- '%c'\n",
                    argv[0], arg[1]);
            fprintf(stderr, "Tente '%s -h' para ajuda.\n", argv[0]);
            return 1;
        }
    }

    /* Abre o arquivo (ou usa stdin). */
    FILE *in;
    int is_stdin = (path == NULL);

    if (is_stdin) {
        in = stdin;
    } else {
        in = fopen(path, "rb");
        if (!in) {
            fprintf(stderr, "%s: %s: nao foi possivel abrir\n",
                    argv[0], path);
            return 1;
        }
    }

    /* Executa. */
    int rc = hexdump_stream(in, stdout, &opts);

    if (!is_stdin)
        fclose(in);

    return (rc == 0) ? 0 : 1;
}