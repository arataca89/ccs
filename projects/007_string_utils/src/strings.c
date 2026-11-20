#include "strings.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/* Helper: verifica se `c` é um espaço em branco (para `strtrim`). */
static int is_space(int c)
{
    return c == ' ' || c == '\t' || c == '\n' ||
           c == '\r' || c == '\v' || c == '\f';
}

char *strdup(const char *s)
{
    if (!s)
        return NULL;

    size_t len = strlen(s) + 1;   /* +1 para o '\0' */
    char *copy = malloc(len);
    if (!copy)
        return NULL;

    memcpy(copy, s, len);
    return copy;
}

char *strndup(const char *s, size_t n)
{
    if (!s)
        return NULL;

    size_t len = strlen(s);
    if (len > n)
        len = n;

    char *copy = malloc(len + 1);
    if (!copy)
        return NULL;

    memcpy(copy, s, len);
    copy[len] = '\0';
    return copy;
}

char *strtrim(const char *s)
{
    if (!s)
        return NULL;

    /* Pula espaços do início. */
    const char *start = s;
    while (*start && is_space((unsigned char)*start))
        start++;

    /* Se a string é só espaços, retorna uma string vazia. */
    if (*start == '\0')
        return strdup("");

    /* Encontra o fim (último não-espaço). */
    const char *end = start + strlen(start) - 1;
    while (end > start && is_space((unsigned char)*end))
        end--;

    /* `end` aponta para o último não-espaço. A string vai de
     * `start` a `end` (inclusive), com `end - start + 1`
     * caracteres. */
    size_t len = (size_t)(end - start + 1);
    char *trimmed = malloc(len + 1);
    if (!trimmed)
        return NULL;

    memcpy(trimmed, start, len);
    trimmed[len] = '\0';
    return trimmed;
}

void str_reverse(char *s)
{
    if (!s)
        return;

    size_t len = strlen(s);
    if (len < 2)
        return;

    for (size_t i = 0, j = len - 1; i < j; i++, j--) {
        char tmp = s[i];
        s[i] = s[j];
        s[j] = tmp;
    }
}

void strtolower(char *s)
{
    if (!s)
        return;

    for (; *s; s++)
        *s = (char)tolower((unsigned char)*s);
}

void strtoupper(char *s)
{
    if (!s)
        return;

    for (; *s; s++)
        *s = (char)toupper((unsigned char)*s);
}

char **strsplit(const char *s, char delim)
{
    if (!s)
        return NULL;

    /* Conta quantos tokens (delimitadores + 1). */
    size_t count = 1;
    for (const char *p = s; *p; p++) {
        if (*p == delim)
            count++;
    }

    /* Aloca o array de ponteiros (count + 1 para o NULL). */
    char **tokens = malloc((count + 1) * sizeof(char *));
    if (!tokens)
        return NULL;

    /* Preenche os tokens. */
    size_t i = 0;
    const char *start = s;

    for (const char *p = s; ; p++) {
        if (*p == delim || *p == '\0') {
            size_t len = (size_t)(p - start);
            tokens[i] = malloc(len + 1);
            if (!tokens[i]) {
                /* Erro: libera os tokens já alocados. */
                for (size_t j = 0; j < i; j++)
                    free(tokens[j]);
                free(tokens);
                return NULL;
            }
            memcpy(tokens[i], start, len);
            tokens[i][len] = '\0';
            i++;

            if (*p == '\0')
                break;
            start = p + 1;
        }
    }

    tokens[i] = NULL;   /* termina o array */
    return tokens;
}

char *strjoin(char *const *parts, const char *sep)
{
    if (!parts)
        return NULL;
    if (!sep)
        sep = "";

    /* Calcula o tamanho total. */
    size_t total = 0;
    size_t n_parts = 0;
    for (char *const *p = parts; *p; p++) {
        total += strlen(*p);
        n_parts++;
    }
    if (n_parts == 0)
        return strdup("");

    total += (n_parts - 1) * strlen(sep);

    /* Aloca a string final. */
    char *joined = malloc(total + 1);
    if (!joined)
        return NULL;

    /* Copia as partes e os separadores. */
    char *dst = joined;
    for (char *const *p = parts; *p; p++) {
        size_t len = strlen(*p);
        memcpy(dst, *p, len);
        dst += len;
        if (*(p + 1) != NULL) {   /* não é a última parte */
            size_t sep_len = strlen(sep);
            memcpy(dst, sep, sep_len);
            dst += sep_len;
        }
    }
    *dst = '\0';

    return joined;
}