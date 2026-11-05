#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "grep.h"

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

static void setup_binary_stdio(void)
{
#ifdef _WIN32
    _setmode(_fileno(stdin),  _O_BINARY);
    _setmode(_fileno(stdout), _O_BINARY);
#endif
}

static void print_help(void)
{
    printf(
        "Uso: grep [OPCOES] PADRAO [ARQUIVO...]\n"
        "\n"
        "Busca PADRAO (literal) nas linhas de ARQUIVOs. Sem ARQUIVO,\n"
        "le de stdin. Use '-' como nome de arquivo para ler de stdin\n"
        "explicitamente.\n"
        "\n"
        "Opcoes:\n"
        "  -i    ignora maiusculas/minusculas\n"
        "  -v    inverte: mostra linhas que NAO casam\n"
        "  -c    conta linhas que casam (nao imprime)\n"
        "  -n    mostra numero da linha\n"
        "  -l    mostra so o nome do arquivo, se houver match\n"
        "  -U    remove '\\r' no fim de linha (converte CRLF em LF)\n"
        "  -h    mostra esta ajuda e sai\n"
        "\n"
        "Exemplos:\n"
        "  grep compilacao arquivo.txt\n"
        "  grep -in compilacao arquivo.txt\n"
        "  echo ola | grep ola\n"
        "\n"
        "Nota sobre arquivos CRLF (Windows):\n"
        "  O grep busca bytes fielmente. Arquivos com quebras \\r\\n\n"
        "  (Windows) preservam o '\\r' na saida, o que faz o cursor\n"
        "  voltar ao inicio da linha no terminal (a linha 'parece'\n"
        "  bagunçada). Isso NAO e bug: e o comportamento do grep\n"
        "  POSIX. Para ver a saida limpa, use -U:\n"
        "    grep -U padrao arquivo.txt\n"
    );
}

/* Percorre argv e liga as flags. Para no primeiro argumento que
 * não é flag (que será o PADRÃO). Retorna 0 em sucesso, -1 se
 * uma flag for desconhecida. */
static int parse_flags(int argc, char **argv, GrepOptions *opts,
                       int *first_non_flag)
{
    *first_non_flag = argc;

    for (int i = 1; i < argc; i++) {
        const char *arg = argv[i];

        /* "-" sozinho é stdin, não é flag. */
        if (arg[0] != '-' || arg[1] == '\0') {
            *first_non_flag = i;
            return 0;
        }

        for (const char *p = arg + 1; *p != '\0'; p++) {
            switch (*p) {
            case 'i': opts->ignore_case        = 1; break;
            case 'v': opts->invert_match       = 1; break;
            case 'c': opts->count_only         = 1; break;
            case 'n': opts->line_number        = 1; break;
            case 'l': opts->files_with_matches = 1; break;
            case 'U': opts->convert_crlf       = 1; break;
            case 'h':
                print_help();
                exit(0);
            default:
                fprintf(stderr, "grep: opcao invalida -- '%c'\n", *p);
                fprintf(stderr, "Tente 'grep -h' para ajuda.\n");
                return -1;
            }
        }
    }

    return 0;
}

/* Processa um arquivo (ou stdin, se path == NULL ou "-").
 * `filename` passado ao grep_stream é sempre o nome real (ou NULL
 * para stdin), porque o -l precisa dele. O prefixo de linha é
 * controlado por opts->show_filename, não por filename. */
static int process_one(const char *path, const char *pattern,
                       const GrepOptions *opts,
                       unsigned long *total_matches,
                       int *had_error)
{
    FILE *in;
    int is_stdin = (path == NULL || strcmp(path, "-") == 0);

    if (is_stdin) {
        in = stdin;
    } else {
        in = fopen(path, "rb");
        if (!in) {
            fprintf(stderr, "grep: %s: nao foi possivel abrir\n", path);
            *had_error = 1;
            return -1;
        }
    }

    unsigned long matches = 0;
    int rc = grep_stream(in, is_stdin ? NULL : path, pattern, opts, &matches);

    if (!is_stdin)
        fclose(in);

    if (rc != 0) {
        *had_error = 1;
        return -1;
    }

    *total_matches += matches;
    return 0;
}

int main(int argc, char **argv)
{
    setup_binary_stdio();

    GrepOptions opts = {0};
    int first_non_flag = argc;

    if (parse_flags(argc, argv, &opts, &first_non_flag) != 0)
        return 2;

    if (first_non_flag >= argc) {
        fprintf(stderr, "grep: falta o PADRAO\n");
        fprintf(stderr, "Tente 'grep -h' para ajuda.\n");
        return 2;
    }

    const char *pattern = argv[first_non_flag];
    int first_file = first_non_flag + 1;
    int n_files = argc - first_file;

    /* Prefixo de linha só quando há múltiplos arquivos. */
    opts.show_filename = (n_files > 1);

    int had_error = 0;
    unsigned long total_matches = 0;

    if (n_files == 0) {
        if (process_one(NULL, pattern, &opts,
                        &total_matches, &had_error) != 0)
            return 2;
    } else {
        for (int i = first_file; i < argc; i++) {
            process_one(argv[i], pattern, &opts,
                        &total_matches, &had_error);
        }
    }

    if (had_error)
        return 2;

    return (total_matches > 0) ? 0 : 1;
}