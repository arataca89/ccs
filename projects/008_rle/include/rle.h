#ifndef RLE_H
#define RLE_H

#include <stdio.h>

/* Comprime `in` para `out`, no formato RLE: pares (byte, contagem).
 * A ordem dos parâmetros segue a convenção da biblioteca C
 * (destino antes da origem), como em `strcpy(dest, src)`.
 *
 * Retorna 0 em sucesso, -1 em erro de leitura/escrita. */
int rle_compress(FILE *out, FILE *in);

/* Descomprime `in` para `out`.
 *
 * Retorna 0 em sucesso, -1 em erro de leitura/escrita ou arquivo
 * corrompido (número ímpar de bytes, ou contagem zero). */
int rle_decompress(FILE *out, FILE *in);

#endif /* RLE_H */