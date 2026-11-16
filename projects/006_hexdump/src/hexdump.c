#include "hexdump.h"

/* Tamanho do bloco lido por vez. 16 é o padrão do hexdump -C. */
#define BLOCK 16

/* Verifica se `c` é um caractere ASCII imprimível.
 * Usamos verificação manual em vez de isprint() para evitar
 * dependência de locale. */
static int is_printable(unsigned char c)
{
    /* 32 = espaco (0x20), 126 = ~ (0x7E) */
    return c >= 32 && c <= 126;
}

/* Imprime uma linha do hexdump: offset, 16 bytes em hex, ASCII. */
static void print_line(unsigned long offset,
                       const unsigned char *buf, size_t n,
                       FILE *out)
{
    /* Offset: 8 dígitos hex, zero-padded. */
    fprintf(out, "%08lx  ", offset);

    /* Bytes em hex: dois grupos de 8, separados por espaço. */
    for (size_t i = 0; i < BLOCK; i++) {
        if (i == 8)
            fputc(' ', out);   /* espaço extra no meio */
        if (i < n)
            fprintf(out, "%02x ", buf[i]);
        else
            fprintf(out, "   ");   /* padding para bytes faltantes */
    }

    /* ASCII: bytes imprimíveis, ou '.' para não-imprimíveis. */
    fputc('|', out);
    for (size_t i = 0; i < n; i++) {
        unsigned char c = buf[i];
        fputc(is_printable(c) ? c : '.', out);
    }
    fputc('|', out);
    fputc('\n', out);
}

int hexdump_stream(FILE *in, FILE *out, const HexdumpOptions *opts)
{
    /* Pular `skip` bytes, se pedido. */
    if (opts->skip > 0) {
        if (fseek(in, (long)opts->skip, SEEK_SET) != 0) {
            fprintf(stderr, "hexdump: nao foi possivel pular %lu bytes\n",
                    opts->skip);
            return -1;
        }
    }

    unsigned char buf[BLOCK];
    unsigned long offset = opts->skip;
    unsigned long total = 0;
    size_t n;

    while ((n = fread(buf, 1, BLOCK, in)) > 0) {
        /* Se há limite e já lemos o suficiente, para. */
        if (opts->limit > 0 && total + n > opts->limit)
            n = (size_t)(opts->limit - total);

        print_line(offset, buf, n, out);

        offset += n;
        total += n;

        if (opts->limit > 0 && total >= opts->limit)
            break;
    }

    if (ferror(in)) {
        fprintf(stderr, "hexdump: erro de leitura\n");
        return -1;
    }

    return 0;
}