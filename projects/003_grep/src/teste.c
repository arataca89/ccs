/* teste temporário — não vai para o repo.
 * Exercita o grep_stream com combinações fixas de flags,
 * lendo de um arquivo passado como argumento.
 *
 * Uso: teste <arquivo>
 *      teste <arquivo> -i
 *      teste <arquivo> -v
 *      ... (qualquer combinação de i, v, c, n, l, U)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "grep.h"

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "uso: %s <arquivo> [flags...]\n", argv[0]);
        return 2;
    }

    const char *path = argv[1];

    /* Flags fixas no código, para testar. */
    GrepOptions opts = {0};
    for (int i = 2; i < argc; i++) {
        for (const char *p = argv[i]; *p; p++) {
            switch (*p) {
            case 'i': opts.ignore_case        = 1; break;
            case 'v': opts.invert_match       = 1; break;
            case 'c': opts.count_only         = 1; break;
            case 'n': opts.line_number        = 1; break;
            case 'l': opts.files_with_matches = 1; break;
            case 'U': opts.convert_crlf       = 1; break;
            default:
                fprintf(stderr, "flag desconhecida: %c\n", *p);
                return 2;
            }
        }
    }

    /* Padrão fixo para teste: "compilacao". */
    const char *pattern = "compilacao";

    FILE *in = fopen(path, "rb");
    if (!in) {
        fprintf(stderr, "nao foi possivel abrir: %s\n", path);
        return 2;
    }

    unsigned long matches = 0;
    int rc = grep_stream(in, path, pattern, &opts, &matches);
    fclose(in);

    if (rc != 0)
        return 2;

    return matches > 0 ? 0 : 1;
}