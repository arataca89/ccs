#ifndef GREP_H
#define GREP_H

#include <stdio.h>

/* Opções de busca do grep. Agrupadas numa struct para passar como
 * um único parâmetro e deixar claro o que é "configuração". */
typedef struct {
    int ignore_case;         /* -i: ignora maiúsculas/minúsculas */
    int invert_match;        /* -v: imprime linhas que NÃO casam */
    int count_only;          /* -c: conta linhas que casam, não imprime */
    int line_number;         /* -n: mostra número da linha */
    int files_with_matches;  /* -l: mostra só o nome do arquivo */
    int convert_crlf;        /* -U: remove \r no fim de linha */
    int show_filename;       /* prefixar linhas com nome do arquivo? */
} GrepOptions;

/* Lê uma linha de `f`, alocando/crescendo `*buf` conforme necessário.
 * `*cap` é a capacidade atual do buffer. O buffer é reutilizado entre
 * chamadas (não aloca um novo a cada linha).
 *
 * Retorna o número de caracteres lidos (sem o '\n'), ou:
 *   -1 em EOF (nenhum caractere lido)
 *   -2 em erro de alocação ou leitura
 *
 * A linha retornada em `*buf` é sempre terminada em '\0'. O '\n' é
 * removido (se presente). */
long read_line(FILE *f, char **buf, size_t *cap, int strip_cr);

/* Busca `pattern` em `line`, respeitando `ignore_case`.
 * Retorna 1 se casou, 0 se não casou. */
int line_matches(const char *line, const char *pattern, int ignore_case);

/* Processa um stream, imprimindo as linhas que casam (ou contando,
 * se -c). `filename` é usado para o prefixo (quando há múltiplos
 * arquivos) e para -l.
 *
 * Retorna 0 em sucesso, -1 em erro. `*match_count` recebe o número
 * de linhas que casaram. */
int grep_stream(FILE *in, const char *filename,
                const char *pattern,
                const GrepOptions *opts,
                unsigned long *match_count);

#endif /* GREP_H */