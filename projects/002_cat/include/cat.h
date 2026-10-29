#ifndef CAT_H
#define CAT_H

#include <stdio.h>

/* Opções de transformação do cat. Agrupadas numa struct para passar
 * como um único parâmetro e deixar claro o que é "configuração". */
typedef struct {
    int number_all;        /* -n: numera todas as linhas */
    int number_nonblank;   /* -b: numera só linhas não vazias */
    int show_ends;         /* -E: mostra '$' no fim de cada linha */
    int show_tabs;         /* -T: mostra tabs como '^I' */
    int squeeze_blank;     /* -s: colapsa linhas em branco múltiplas */
    int convert_crlf;      /* -U: converte \r\n em \n na saída */
} CatOptions;

/* Copia `input` para `output`, aplicando as opções.
 * Retorna 0 em sucesso, -1 em erro de leitura ou escrita. */
int cat_copy(FILE *input, FILE *output, const CatOptions *opts);

#endif /* CAT_H */
