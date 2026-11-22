#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rle.h"

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

static void setup_binary_stdio(void)
{
#ifdef _WIN32
    _setmode(_fileno(stdin),  _O_BINARY);
    _setmode(_fileno(stdout), _O_BINARY);
#endif
}

static void print_help(const char *prog)
{
    printf(
        "Uso: %s [OPCOES] [ARQUIVO]\n"
        "\n"
        "Comprime ou descomprime ARQUIVO usando RLE (Run-Length\n"
        "Encoding). Sem ARQUIVO, le de stdin.\n"
        "\n"
        "Opcoes:\n"
        "  -c    comprime (padrao)\n"
        "  -d    descomprime\n"
        "  -h    mostra esta ajuda e sai\n"
        "\n"
        "Exemplos:\n"
        "  %s -c entrada.txt > saida.rle\n"
        "  %s -d saida.rle > entrada.txt\n"
        "  type entrada.txt | %s -c > saida.rle\n",
        prog, prog, prog, prog
    );
}

int main(int argc, char **argv)
{
    setup_binary_stdio();

    int decompress = 0;
    const char *path = NULL;

    /* Parsing manual de flags. */
    for (int i = 1; i < argc; i++) {
        const char *arg = argv[i];

        if (arg[0] != '-' || arg[1] == '\0') {
            /* Não é flag: é o arquivo. */
            path = arg;
            continue;
        }

        switch (arg[1]) {
        case 'c':
            decompress = 0;
            break;
        case 'd':
            decompress = 1;
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
    int rc;
    if (decompress)
        rc = rle_decompress(stdout, in);
    else
        rc = rle_compress(stdout, in);

    if (!is_stdin)
        fclose(in);

    return (rc == 0) ? 0 : 1;
}