#include "grep.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/* Cresce o buffer para pelo menos `min_cap` bytes.
 * Retorna 0 em sucesso, -1 em erro de alocação.
 *
 * Cuidado clássico: NÃO fazer `buf = realloc(buf, ...)` diretamente,
 * porque se realloc falhar, perdemos o ponteiro original (memory leak).
 * Usar um ponteiro temporário. */
static int grow_buffer(char **buf, size_t *cap, size_t min_cap)
{
    if (*cap >= min_cap)
        return 0;

    size_t new_cap = *cap ? *cap : 64;
    while (new_cap < min_cap)
        new_cap *= 2;   /* doubling: O(1) amortizado */

    char *tmp = realloc(*buf, new_cap);
    if (!tmp)
        return -1;

    *buf = tmp;
    *cap = new_cap;
    return 0;
}

long read_line(FILE *f, char **buf, size_t *cap, int strip_cr)
{
    if (!*buf || *cap == 0) {
        if (grow_buffer(buf, cap, 64) != 0)
            return -2;
    }

    size_t len = 0;
    int c;

    while ((c = fgetc(f)) != EOF) {
        if (c == '\n')
            break;

        if (len + 1 >= *cap) {
            if (grow_buffer(buf, cap, *cap * 2) != 0)
                return -2;
        }

        (*buf)[len++] = (char)c;
    }

    if (c == EOF && len == 0)
        return -1;

    /* Remove '\r' no final, se -U e a linha termina em \r\n. */
    if (strip_cr && len > 0 && (*buf)[len - 1] == '\r')
        len--;

    (*buf)[len] = '\0';

    if (ferror(f))
        return -2;

    return (long)len;
}

/* Versão case-insensitive de strstr.
 * Não é C padrão (é extensão GNU/BSD), então implementamos. */
static const char *strcasestr_impl(const char *haystack, const char *needle)
{
    if (!*needle)
        return haystack;

    for (; *haystack; haystack++) {
        const char *h = haystack;
        const char *n = needle;

        while (*h && *n && tolower((unsigned char)*h) == tolower((unsigned char)*n)) {
            h++;
            n++;
        }

        if (!*n)
            return haystack;
    }

    return NULL;
}

int line_matches(const char *line, const char *pattern, int ignore_case)
{
    if (ignore_case)
        return strcasestr_impl(line, pattern) != NULL;
    return strstr(line, pattern) != NULL;
}

int grep_stream(FILE *in, const char *filename,
                const char *pattern,
                const GrepOptions *opts,
                unsigned long *match_count)
{
    char *buf = NULL;
    size_t cap = 0;
    unsigned long line_no = 0;
    unsigned long matches = 0;
    int rc = 0;

    long len;
    while ((len = read_line(in, &buf, &cap, opts->convert_crlf)) >= 0) {
        line_no++;

        int matched = line_matches(buf, pattern, opts->ignore_case);
        if (opts->invert_match)
            matched = !matched;

        if (matched) {
            matches++;

            if (opts->files_with_matches) {
                /* -l: mostra só o nome do arquivo e para. */
                printf("%s\n", filename ? filename : "(standard input)");
                break;
            }

            if (!opts->count_only) {
                if (opts->show_filename && filename)
                    printf("%s:", filename);
                if (opts->line_number)
                    printf("%lu:", line_no);
                printf("%s\n", buf);
            }


        }
    }

    if (len == -2) {
        fprintf(stderr, "grep: erro de leitura em %s\n",
                filename ? filename : "(standard input)");
        rc = -1;
    }

    if (opts->count_only && !opts->files_with_matches)
        printf("%lu\n", matches);

    if (match_count)
        *match_count = matches;

    free(buf);
    return rc;
}