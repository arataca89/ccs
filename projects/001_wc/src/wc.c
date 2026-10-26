#include "wc.h"

int wc_count(FILE *stream, WcCounts *out)
{
    out->lines = 0;
    out->words = 0;
    out->bytes = 0;

    int c;
    int in_word = 0;      /* estamos dentro de uma "palavra"? */

    while ((c = fgetc(stream)) != EOF){
        out->bytes++;

        if (c == '\n')
            out->lines++;

        /* isspace() do jeito clássico de C, mas feito à mão para
         * não depender de <ctype.h> com locale. */
        int is_space = (c == ' ' || c == '\t' || c == '\n' ||
                        c == '\r' || c == '\v' || c == '\f');

        if (is_space){
            in_word = 0;
        } else if (!in_word){
            in_word = 1;
            out->words++;
        }
    }

    if (ferror(stream))
        return -1;

    return 0;
}