#ifndef HEXDUMP_H
#define HEXDUMP_H

#include <stdio.h>

/* Opções do hexdump. */
typedef struct {
    unsigned long skip;   /* -s N: pular N bytes no inicio */
    unsigned long limit;  /* -n N: ler no maximo N bytes (0 = sem limite) */
} HexdumpOptions;

/* Le `in` e escreve o hexdump em `out`, aplicando as opcoes.
 * Retorna 0 em sucesso, -1 em erro de leitura/escrita. */
int hexdump_stream(FILE *in, FILE *out, const HexdumpOptions *opts);

#endif /* HEXDUMP_H */