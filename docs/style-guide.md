# Guia de estilo do CCS

Regras de código para todos os projetos do CCS. O objetivo não é
"o melhor estilo do mundo" — é **consistência**. Um leitor que
abre o projeto 001 e o projeto 002 deve reconhecer o mesmo jeito
de escrever C.

## Versão da linguagem

- **C11**, com `CMAKE_C_EXTENSIONS OFF`. Nada de extensões de
  compilador (GNU, MSVC) a menos que estritamente necessário e
  documentado com `#ifdef`.
- Compilar limpo com `-Wall -Wextra -Wpedantic` (GCC/Clang) e
  `/W4` (MSVC). Warning não é sugestão.

## Formatação

- **Indentação**: 4 espaços. Não use tabs.
- **Chaves em funções**: linha seguinte (Allman).
- **Chaves em controle de fluxo** (`if`, `for`, `while`, `switch`):
  mesma linha (K&R).
- **Chaves em `struct`/`enum`/`union`**: mesma linha (K&R).
- **Comprimento de linha**: até 80 colunas.
- **Espaços**: um depois de vírgula, nenhum antes. Um ao redor de
  operadores binários, nenhum ao redor de unários.

```c
/* Função: chave na linha seguinte */
int wc_count(FILE *stream, WcCounts *out)
{
    /* Controle de fluxo: chave na mesma linha */
    if (c == '\n') {
        out->lines++;
    } else if (is_space) {
        in_word = 0;
    }

    /* Struct: chave na mesma linha */
    struct point {
        int x;
        int y;
    };

    return 0;
}
