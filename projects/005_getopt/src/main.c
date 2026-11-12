#include <stdio.h>
#include <stdlib.h>

#include "getopt.h"

static void print_help(const char *prog)
{
    printf(
        "Uso: %s [OPCOES] [ARQUIVO...]\n"
        "\n"
        "Demonstra o getopt. Processa as flags e mostra os resultados.\n"
        "\n"
        "Opcoes:\n"
        "  -v          modo verboso (pode ser repetida: -vv)\n"
        "  -o ARQUIVO  arquivo de saida (aceita -o arquivo ou -oarquivo)\n"
        "  -h          mostra esta ajuda e sai\n"
        "\n"
        "Exemplos:\n"
        "  %s -v arquivo.txt\n"
        "  %s -vo saida.txt arquivo.txt\n"
        "  %s -vosaida.txt arquivo.txt\n"
        "  %s -vv -o saida.txt a.txt b.txt\n"
        "\n"
        "Nota: as flags podem ser agrupadas (-vo) e o argumento de -o\n"
        "pode ser colado (-osaida.txt) ou separado (-o saida.txt).\n"
        "Use '--' para parar o parsing de flags.\n",
        prog, prog, prog, prog, prog
    );
}

int main(int argc, char *argv[])
{
    int verbose = 0;
    const char *output = NULL;

    int opt;
    while ((opt = getopt(argc, argv, "vo:h")) != -1) {
        switch (opt) {
        case 'v':
            verbose++;
            break;
        case 'o':
            output = optarg;
            break;
        case 'h':
            print_help(argv[0]);
            return 0;
        case '?':
            /* getopt já imprimiu a mensagem (se opterr != 0). */
            fprintf(stderr, "Tente '%s -h' para ajuda.\n", argv[0]);
            return 1;
        }
    }

    /* Mostra os resultados. */
    printf("verbose = %d\n", verbose);
    printf("output  = %s\n", output ? output : "(nenhum)");

    if (optind < argc) {
        printf("arquivos:\n");
        for (int i = optind; i < argc; i++)
            printf("  %s\n", argv[i]);
    } else {
        printf("arquivos: (nenhum)\n");
    }

    return 0;
}