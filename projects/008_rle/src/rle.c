#include "rle.h"

#include <stdio.h>

#define MAX_COUNT 255   /* a contagem é 1 byte (0-255) */

int rle_compress(FILE *out, FILE *in)
{
    int current;
    int previous;

    previous = fgetc(in);
    if (previous == EOF) {
        /* Arquivo vazio: nada a fazer. */
        if (ferror(in))
            return -1;
        return 0;
    }

    unsigned count = 1;

    while ((current = fgetc(in)) != EOF) {
        if (current == previous) {
            count++;
            if (count == MAX_COUNT) {
                /* Sequência atingiu 255: emite o par e recomeça. */
                if (fputc(previous, out) == EOF) return -1;
                if (fputc((int)count, out) == EOF) return -1;
                count = 0;   /* será 1 quando o próximo `current` for lido */
            }
        } else {
            /* Sequência terminou: emite o par. */
            if (fputc(previous, out) == EOF) return -1;
            if (fputc((int)count, out) == EOF) return -1;
            previous = current;
            count = 1;
        }
    }

    if (ferror(in))
        return -1;

    /* Emite o último par (a sequência que estava em andamento). */
    if (count > 0) {
        if (fputc(previous, out) == EOF) return -1;
        if (fputc((int)count, out) == EOF) return -1;
    }

    return 0;
}

int rle_decompress(FILE *out, FILE *in)
{
    int byte;
    int count;

    while ((byte = fgetc(in)) != EOF) {
        count = fgetc(in);
        if (count == EOF) {
            /* Número ímpar de bytes: arquivo corrompido. */
            fprintf(stderr, "rle: arquivo corrompido (falta a contagem)\n");
            return -1;
        }
        if (count == 0) {
            fprintf(stderr, "rle: arquivo corrompido (contagem zero)\n");
            return -1;
        }

        for (int i = 0; i < count; i++) {
            if (fputc(byte, out) == EOF)
                return -1;
        }
    }

    if (ferror(in))
        return -1;

    return 0;
}