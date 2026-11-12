#ifndef GETOPT_H
#define GETOPT_H

/* Variáveis globais do getopt (POSIX). */
extern char *optarg;   /* argumento da flag atual (se a flag espera um) */
extern int optind;     /* índice do próximo argumento a processar */
extern int opterr;     /* se 0, getopt não imprime mensagens de erro */
extern int optopt;     /* a flag inválida (quando getopt retorna '?') */

/* Faz parsing de flags de linha de comando.
 *
 * `argc` e `argv` são os argumentos do main.
 * `optstring` descreve as flags: cada caractere é uma flag; se
 * seguido de ':', a flag espera um argumento.
 *
 * Retorna o caractere da próxima flag, ou -1 quando não há mais.
 * Retorna '?' em flag inválida (e seta optopt).
 *
 * Variáveis globais:
 *   optarg — argumento da flag (se a flag espera um)
 *   optind — índice do próximo argumento não-flag
 *   opterr — se 0, não imprime erros (padrão: 1)
 *   optopt — a flag inválida (quando retorna '?')
 */
int getopt(int argc, char *const argv[], const char *optstring);

#endif /* GETOPT_H */