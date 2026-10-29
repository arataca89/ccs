#include "cat.h"

/* Estado da linha atual, usado para numeração e squeeze.
 * Agrupa variáveis que só fazem sentido juntas. */
typedef struct {
    int at_line_start;      /* estamos no início de uma linha? */
    unsigned long line_no;  /* número da próxima linha a numerar */
    int pending_number;     /* com -b: número adiado, aguardando
                             * saber se a linha é vazia */
    int last_was_blank;     /* com -s: a linha anterior era vazia? */
    int last_was_cr;        /* -U: o caractere anterior foi '\r'? */
} CatState;

/* Emite o número de linha formatado (6 colunas, alinhado à direita,
 * seguido de tab), no formato do cat POSIX. */
static void emit_line_number(unsigned long n, FILE *out)
{
    fprintf(out, "%6lu\t", n);
}

/* Processa um único caractere, aplicando as transformações de
 * saída (-E, -T). Retorna 0 em sucesso, -1 em erro de escrita. */
static int emit_char(int c, FILE *out, const CatOptions *opts)
{
    if (c == '\n' && opts->show_ends) {
        if (fputc('$', out) == EOF) return -1;
        if (fputc('\n', out) == EOF) return -1;
        return 0;
    }
    else if (c == '\t' && opts->show_tabs) {
        if (fputc('^', out) == EOF) return -1;
        if (fputc('I', out) == EOF) return -1;
        return 0;
    }
    return (fputc(c, out) == EOF) ? -1 : 0;
}

int cat_copy(FILE *input, FILE *output, const CatOptions *opts)
{
    CatState st = {
        .at_line_start = 1,
        .line_no = 1,
        .pending_number = 0,
        .last_was_blank = 0,
        .last_was_cr = 0,
    };

    int c;
    while ((c = fgetc(input)) != EOF) {
        int was_at_line_start = st.at_line_start;   // captura ANTES de tudo

        /* --- conversão CRLF (-U) --- */
        /* Estratégia: quando vemos '\r', não emitimos nada ainda. Marcamos
         * last_was_cr. No próximo caractere:
         *   - se for '\n', o '\r' era parte de CRLF: descartamos o '\r' e
         *     processamos o '\n' normalmente.
         *   - se for outra coisa (ou EOF), o '\r' era sozinho: emitimos o
         *     '\r' pendente e processamos o caractere atual.
         * Isso exige "olhar um caractere à frente" (lookahead de 1),
         * padrão comum em parsers. */
        if (opts->convert_crlf) {
            if (c == '\r') {
                st.last_was_cr = 1;
                continue;   /* não emite ainda; decide no próximo caractere */
            }
            if (st.last_was_cr) {
                st.last_was_cr = 0;
                if (c != '\n') {
                    /* '\r' sozinho: emite o '\r' que ficou pendente */
                    if (emit_char('\r', output, opts) != 0)
                        return -1;
                }
                /* se c == '\n', o '\r' era de CRLF: simplesmente o descartamos */
            }
        }

        /* --- squeeze (-s) --- */
        if (opts->squeeze_blank && c == '\n' && st.last_was_blank) {
            /* Linha atual é vazia E a anterior também: suprime. */
            /* Não atualiza at_line_start nem last_was_blank:
             * continuamos "no início de uma linha vazia". */
            continue;
        }

        /* --- início de linha: numeração (-n / -b) --- */
        if (st.at_line_start) {
            st.at_line_start = 0;
            if (opts->number_nonblank) {
                /* -b: só numera se a linha tiver conteúdo.
                 * Adiamos: só sabemos se é vazia ao ver o próximo
                 * caractere. Se for '\n', a linha é vazia. */
                if (c != '\n')
                    st.pending_number = 1;
            } else if (opts->number_all) {
                emit_line_number(st.line_no++, output);
            }
        }

        /* --- resolve numeração adiada (-b) --- */
        if (st.pending_number) {
            /* Chegamos aqui com o primeiro caractere da linha em mãos
             * e sabemos que não é '\n' (senão não teríamos marcado
             * pending_number). Então a linha tem conteúdo. */
            emit_line_number(st.line_no++, output);
            st.pending_number = 0;
        }

        /* --- transformações de saída (-E / -T) --- */
        if (emit_char(c, output, opts) != 0)
            return -1;

        if (c == '\n') {
            st.last_was_blank = was_at_line_start;
            st.at_line_start = 1;
        } else {
            st.at_line_start = 0;
            st.last_was_blank = 0;
        }
    }

    /* Se o arquivo terminou com '\r' e -U está ativo, o '\r' ficou
     * pendente. Emita-o (era '\r' sozinho, não CRLF). */
    if (opts->convert_crlf && st.last_was_cr) {
        if (emit_char('\r', output, opts) != 0)
            return -1;
    }

    return ferror(input) ? -1 : 0;
}
