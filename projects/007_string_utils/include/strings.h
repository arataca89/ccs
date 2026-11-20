#ifndef STRINGS_H
#define STRINGS_H

#include <stddef.h>

/* Duplica `s` (aloca + copia). Retorna a cópia, ou NULL em erro.
 * O chamador deve liberar com free. */
char *strdup(const char *s);

/* Duplica até `n` caracteres de `s`. Retorna a cópia, ou NULL.
 * O chamador deve liberar com free. */
char *strndup(const char *s, size_t n);

/* Remove espaços do início e do fim de `s`, retornando uma NOVA
 * string alocada. O chamador deve liberar com free. */
char *strtrim(const char *s);

/* Inverte `s` in-place. `s` deve ser mutável (não literal). */
void str_reverse(char *s);

/* Converte `s` para minúsculas in-place. */
void strtolower(char *s);

/* Converte `s` para maiúsculas in-place. */
void strtoupper(char *s);

/* Divide `s` por `delim`, retornando um array de strings
 * terminado em NULL. O chamador deve liberar cada string e o
 * array. */
char **strsplit(const char *s, char delim);

/* Junta `parts` (terminado em NULL) com `sep`, retornando uma
 * NOVA string alocada. O chamador deve liberar com free. */
char *strjoin(char *const *parts, const char *sep);

#endif /* STRINGS_H */