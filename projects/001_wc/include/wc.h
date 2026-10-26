#ifndef WC_H
#define WC_H

#include <stddef.h>
#include <stdio.h>

/* Resultado da contagem. Usamos tipos largos porque wc conta bytes,
 * e arquivos grandes estouram int em 32 bits. */
typedef struct {
    unsigned long long lines;
    unsigned long long words;
    unsigned long long bytes;
} WcCounts;

/* Conta linhas, palavras e bytes lendo de `stream`.
 * Retorna 0 em sucesso, -1 em erro de leitura. */
int wc_count(FILE *stream, WcCounts *out);

#endif /* WC_H */
