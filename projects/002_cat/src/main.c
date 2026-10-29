#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cat.h"

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
        "Uso: cat [OPCOES] [ARQUIVO...]\n"
        "\n"
        "Concatena ARQUIVOs para a saida padrao. Sem ARQUIVO, le de stdin.\n"
        "Use '-' como nome de arquivo para ler de stdin explicitamente.\n"
        "\n"
        "Opcoes:\n"
        "  -n    numera todas as linhas\n"
        "  -b    numera apenas linhas nao vazias (sobrepoe -n)\n"
        "  -E    mostra '$' no fim de cada linha\n"
        "  -T    mostra tabs como '^I'\n"
        "  -s    colapsa linhas em branco multiplas em uma\n"
        "  -U    converte quebras \\r\\n (CRLF) em \\n na saida\n"
        "  -h    mostra esta ajuda e sai\n"
        "\n"
        "Exemplos:\n"
        "  cat arquivo.txt\n"
        "  cat -nE arquivo.txt\n"
        "  echo ola | cat -\n"
        "\n"
        "Nota sobre arquivos CRLF (Windows):\n"
        "  O cat copia bytes fielmente. Arquivos com quebras \\r\\n (Windows)\n"
        "  preservam o '\\r', o que afeta o comportamento de -b, -E e -s:\n"
        "    - -E mostra o '$' no lugar errado no terminal (o '\\r' faz o\n"
        "      cursor voltar ao inicio da linha);\n"
        "    - -b numera tambem as linhas vazias (o '\\r' conta como\n"
        "      conteudo);\n"
        "    - -s nao colapsa as linhas em branco como esperado.\n"
        "  Isso NAO e bug: e o comportamento do cat POSIX. Para tratar\n"
        "  CRLF como LF, use -U junto com essas opcoes:\n"
        "    cat -UE arquivo.txt    (mostra '$' no fim de cada linha)\n"
        "    cat -Ub arquivo.txt    (numera so linhas nao vazias)\n"
        "    cat -Us arquivo.txt    (colapsa linhas em branco)\n"
        "\n"
        "Sem argumentos, mostra esta ajuda (o cat POSIX leria de stdin;\n"
        "optamos por mostrar a ajuda por ser mais amigavel no Windows).\n"
    );
}

/* Percorre argv e liga as flags. Aceita flags agrupadas (-nE) e
 * separadas (-n -E). Retorna 0 em sucesso, -1 se uma flag for
 * desconhecida (nesse caso, imprime erro). */
static int parse_flags(int argc, char **argv, CatOptions *opts)
{
    if(argc == 1){
        print_help();
        exit(0);        
    }
    
    for (int i = 1; i < argc; i++) {
        const char *arg = argv[i];

        if (arg[0] != '-' || arg[1] == '\0')
            continue;

        for (const char *p = arg + 1; *p != '\0'; p++) {
            switch (*p) {
            case 'n': opts->number_all      = 1; break;
            case 'b': opts->number_nonblank = 1; break;
            case 'E': opts->show_ends       = 1; break;
            case 'T': opts->show_tabs       = 1; break;
            case 's': opts->squeeze_blank   = 1; break;
            case 'U': opts->convert_crlf    = 1; break;
            case 'h':
                print_help();
                exit(0);
            default:
                fprintf(stderr, "cat: opcao invalida -- '%c'\n", *p);
                fprintf(stderr, "Tente 'cat -h' para ajuda.\n");
                return -1;
            }
        }
    }

    if (opts->number_nonblank)
        opts->number_all = 0;

    return 0;
}

static int process_one(const char *path, const CatOptions *opts)
{
    FILE *in;
    int is_stdin = (path == NULL || strcmp(path, "-") == 0);

    if (is_stdin) {
        in = stdin;
    } else {
        in = fopen(path, "rb");
        if (!in) {
            fprintf(stderr, "cat: %s: nao foi possivel abrir\n", path);
            return -1;
        }
    }

    int rc = cat_copy(in, stdout, opts);

    if (!is_stdin)
        fclose(in);

    if (rc != 0) {
        if (!is_stdin)
            fprintf(stderr, "cat: %s: erro de leitura/escrita\n", path);
        else
            fprintf(stderr, "cat: erro na entrada padrao\n");
        return -1;
    }

    return 0;
}

int main(int argc, char **argv)
{
    setup_binary_stdio();

    CatOptions opts = {0};
    if (parse_flags(argc, argv, &opts) != 0)
        return 1;

    int had_file = 0;
    int had_error = 0;

    for (int i = 1; i < argc; i++) {
        const char *arg = argv[i];
        if (arg[0] == '-' && arg[1] != '\0')
            continue;
        had_file = 1;
        if (process_one(arg, &opts) != 0)
            had_error = 1;
    }

    if (!had_file) {
        if (process_one(NULL, &opts) != 0)
            had_error = 1;
    }

    return had_error ? 1 : 0;
}