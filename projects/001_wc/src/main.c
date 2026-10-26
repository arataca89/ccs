#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wc.h"

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

/* Configura stdin/stdout para modo binário no Windows.
 * Sem isso, \r\n é traduzido para \n e a contagem de bytes fica errada. */
static void setup_binary_stdio(void)
{
#ifdef _WIN32
    _setmode(_fileno(stdin),  _O_BINARY);
    _setmode(_fileno(stdout), _O_BINARY);
#endif
}

static void print_counts(const WcCounts *c, const char *label)
{
    printf("%8llu %8llu %8llu", c->lines, c->words, c->bytes);
    if (label)
        printf(" %s", label);
    putchar('\n');
}

int main(int argc, char **argv)
{
    setup_binary_stdio();

    WcCounts total = {0, 0, 0};
    int had_error = 0;

    if (argc == 1) {
        /* Sem argumentos: lê de stdin, como o wc real. */
        WcCounts c;
        if (wc_count(stdin, &c) != 0) {
            fprintf(stderr, "wc: erro ao ler da entrada padrão\n");
            return 1;
        }
        print_counts(&c, NULL);
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        const char *path = argv[i];

        if (strcmp(path, "-") == 0) {
            /* "-" significa stdin por convenção Unix. */
            WcCounts c;
            if (wc_count(stdin, &c) != 0) {
                fprintf(stderr, "wc: erro ao ler da entrada padrão\n");
                had_error = 1;
                continue;
            }
            print_counts(&c, NULL);
            total.lines += c.lines;
            total.words += c.words;
            total.bytes += c.bytes;
            continue;
        }

        FILE *f = fopen(path, "rb");
        if (!f) {
            fprintf(stderr, "wc: %s: não foi possível abrir\n", path);
            had_error = 1;
            continue;
        }

        WcCounts c;
        int rc = wc_count(f, &c);
        fclose(f);

        if (rc != 0) {
            fprintf(stderr, "wc: %s: erro de leitura\n", path);
            had_error = 1;
            continue;
        }

        print_counts(&c, path);
        total.lines += c.lines;
        total.words += c.words;
        total.bytes += c.bytes;
    }

    if (argc > 2)
        print_counts(&total, "total");

    return had_error ? 1 : 0;
}
