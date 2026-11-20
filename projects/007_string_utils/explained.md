# Explicação detalhada do string_utils

Explicação **linha por linha** do `strings.c` e do `main.c`, para
quem está começando em C e quer entender **o que cada linha faz**
e **por quê**. Não substitui o `notes.md` (que é sobre decisões);
aqui o foco é **didático**.

## Nota sobre `str_reverse` (e não `strrev`)

Originalmente, a função se chamava `strrev`. Mas o MinGW (TDM-GCC)
**já tem** uma `strrev` na `<string.h>`, com assinatura diferente
(`char *strrev(char *)`). O compilador reclamou de "conflicting
types".

Renomeamos para `str_reverse`. O `notes.md` registra a decisão.

## `strings.c`

### Includes

```c
#include "strings.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
```

**`"strings.h"`**: o header do módulo, com as assinaturas das
funções.

**`<ctype.h>`**: para `tolower`, `toupper` (usados em
`strtolower`/`strtoupper`).

**`<stdlib.h>`**: para `malloc`, `free`.

**`<string.h>`**: para `strlen`, `memcpy`.

### `is_space` (helper)

```c
static int is_space(int c)
{
    return c == ' ' || c == '\t' || c == '\n' ||
           c == '\r' || c == '\v' || c == '\f';
}
```

**`static`**: helper interno do `strings.c` (não está no `.h`).

**`int c`**: o parâmetro é `int` (não `char`), porque
`is_space` pode receber `EOF` (que é negativo) em teoria. Na
prática, só recebe chars.

**A função**: retorna 1 (verdadeiro) se `c` é um espaço em
branco, 0 (falso) caso contrário. Cobre os 6 espaços em branco
ASCII: espaço, tab, newline, carriage return, vertical tab, form
feed.

**Por que não `isspace`**: `isspace` depende de `locale` (em
alguns locales, outros caracteres são espaços). Para
**portabilidade**, a verificação manual é mais previsível. É a
mesma decisão do `is_printable` no `006_hexdump`.

### `strdup`

```c
char *strdup(const char *s)
{
    if (!s)
        return NULL;
```

**`if (!s) return NULL`**: se `s` for `NULL`, retorna `NULL`
(não tenta acessar `s`).

```c
    size_t len = strlen(s) + 1;   /* +1 para o '\0' */
```

**`strlen(s) + 1`**: o `strlen` não conta o `'\0'`, mas o
`malloc` precisa do espaço para ele. `+1` garante.

**`size_t`**: o tipo certo para tamanho (ver
[`docs/reference/stddef.md`](../../docs/reference/stddef.md#size_t)).

```c
    char *copy = malloc(len);
    if (!copy)
        return NULL;
```

**`malloc(len)`**: aloca `len` bytes no heap. Retorna `NULL` em
erro.

**`if (!copy) return NULL`**: se a alocação falhar, retorna
`NULL`.

```c
    memcpy(copy, s, len);
    return copy;
}
```

**`memcpy(copy, s, len)`**: copia `len` bytes de `s` para
`copy` (incluindo o `'\0'`). É mais eficiente que `strcpy` (que
para no `'\0'`, mas faz a mesma coisa).

**`return copy`**: retorna o ponteiro para a cópia. O chamador
é responsável por `free`.

### `strndup`

```c
char *strndup(const char *s, size_t n)
{
    if (!s)
        return NULL;

    size_t len = strlen(s);
    if (len > n)
        len = n;
```

**`len` é o mínimo entre `strlen(s)` e `n`**. Se `s` for menor
que `n`, copia a string inteira. Se `s` for maior, copia só `n`
caracteres.

```c
    char *copy = malloc(len + 1);
    if (!copy)
        return NULL;

    memcpy(copy, s, len);
    copy[len] = '\0';
    return copy;
}
```

**`malloc(len + 1)`**: `len` caracteres + 1 para o `'\0'`.

**`memcpy(copy, s, len)`**: copia `len` bytes.

**`copy[len] = '\0'`**: **sempre** termina em `'\0'`, mesmo que
`s` não seja (se `n` for menor que o tamanho de `s`, a cópia tem
`n` caracteres + `'\0'`).

### `strtrim`

```c
char *strtrim(const char *s)
{
    if (!s)
        return NULL;
```

**`if (!s) return NULL`**: se `s` for `NULL`, retorna `NULL`.

```c
    const char *start = s;
    while (*start && is_space((unsigned char)*start))
        start++;
```

**`start`** aponta para o início da string. O `while` avança
enquanto o caractere for espaço. Quando encontra um não-espaço
(ou `'\0'`), para.

**`*start`**: o caractere atual. Se for `'\0'`, o `while` para
(também é falso).

**`(unsigned char)*start`**: cast para `unsigned char`, para
`is_space` funcionar corretamente (ver
[`notes.md`](notes.md) para a explicação sobre `char` signed).

```c
    if (*start == '\0')
        return strdup("");
```

**Se `*start == '\0'`**: a string é **só espaços** (ou vazia).
Retorna `strdup("")` (uma string vazia alocada). O chamador
libera com `free`.

```c
    const char *end = start + strlen(start) - 1;
    while (end > start && is_space((unsigned char)*end))
        end--;
```

**`end = start + strlen(start) - 1`**: `end` aponta para o
**último caractere** da string (não o `'\0'`). `start +
strlen(start)` aponta para o `'\0'`; `- 1` recua para o último
caractere.

**`while (end > start && is_space(...))`**: recua `end`
enquanto for espaço. Quando encontra um não-espaço, ou chega em
`start`, para.

**`end > start`**: garante que não recua além do início (se a
string é só um caractere não-espaço, `end == start`, e o loop
não executa).

```c
    size_t len = (size_t)(end - start + 1);
```

**`end - start`**: a **distância** entre `end` e `start`, em
**elementos** (chars). Retorna `ptrdiff_t` (signed). Ver
[`docs/pointer-arithmetic.md`](../../docs/pointer-arithmetic.md).

**`+ 1`**: o número de caracteres de `start` a `end`,
**inclusive**. O `+1` é porque `end` aponta para o último
caractere (não o próximo).

**`(size_t)`**: converte para `size_t` (o tipo que `malloc`
espera, e evita warning de signed/unsigned).

```c
    char *trimmed = malloc(len + 1);
    if (!trimmed)
        return NULL;

    memcpy(trimmed, start, len);
    trimmed[len] = '\0';
    return trimmed;
}
```

**`malloc(len + 1)`**: `len` caracteres + 1 para o `'\0'`.

**`memcpy(trimmed, start, len)`**: copia `len` bytes a partir de
`start`.

**`trimmed[len] = '\0'`**: termina a string.

**`return trimmed`**: retorna a string trimada. O chamador
libera com `free`.

### `str_reverse`

```c
void str_reverse(char *s)
{
    if (!s)
        return;
```

**`void`**: não retorna nada (modifica `s` in-place).

**`if (!s) return`**: se `s` for `NULL`, não faz nada.

```c
    size_t len = strlen(s);
    if (len < 2)
        return;
```

**`len < 2`**: se a string tem 0 ou 1 caractere, não há o que
inverter. Retorna.

```c
    for (size_t i = 0, j = len - 1; i < j; i++, j--) {
        char tmp = s[i];
        s[i] = s[j];
        s[j] = tmp;
    }
}
```

**Dois índices**: `i` (início) e `j` (fim). A cada iteração,
troca `s[i]` com `s[j]`, e avança `i`, recua `j`.

**`i < j`**: quando `i >= j` (se cruzaram), para. Se a string
tem tamanho ímpar, o caractere do meio não precisa ser trocado
(ele é o mesmo).

**`char tmp`**: variável temporária para a troca (o clássico
"swap" com três variáveis).

### `strtolower`

```c
void strtolower(char *s)
{
    if (!s)
        return;
```

**`void`**: não retorna nada (modifica `s` in-place).

**`if (!s) return`**: se `s` for `NULL`, não faz nada.

```c
    for (; *s; s++)
        *s = (char)tolower((unsigned char)*s);
}
```

**`for (; *s; s++)`**: percorre a string até o `'\0'`. O `*s`
é o caractere atual (condição: diferente de `'\0'`).

**`*s = (char)tolower((unsigned char)*s)`**:
- **`(unsigned char)*s`**: cast para `unsigned char`, para
  `tolower` funcionar corretamente (evita UB, Undefined Behavior, Comportamento Indefinido; com valores > 127).
- **`tolower(...)`**: retorna o caractere em minúscula (se for
  maiúscula), ou o mesmo caractere (se não for).
- **`(char)`**: o retorno de `tolower` é `int`, mas sabemos que
  não é `EOF` (o caractere é válido). O cast para `char` evita
  warning.

**`*s = ...`**: atribui o resultado de volta para `s`.

### `strtoupper`

Idêntico ao `strtolower`, mas usa `toupper` em vez de `tolower`:

```c
void strtoupper(char *s)
{
    if (!s)
        return;

    for (; *s; s++)
        *s = (char)toupper((unsigned char)*s);
}
```

### `strsplit`

```c
char **strsplit(const char *s, char delim)
{
    if (!s)
        return NULL;
```

**`char **`**: retorna um array de strings (terminado em
`NULL`).

**`if (!s) return NULL`**: se `s` for `NULL`, retorna `NULL`.

```c
    size_t count = 1;
    for (const char *p = s; *p; p++) {
        if (*p == delim)
            count++;
    }
```

**Conta os tokens**: o número de delimitadores + 1. Se `s` é
`"a,b,c"`, tem 2 delimitadores, então 3 tokens.

**`count = 1`**: mesmo que `s` não tenha delimitador, tem **1**
token (a string inteira).

```c
    char **tokens = malloc((count + 1) * sizeof(char *));
    if (!tokens)
        return NULL;
```

**`malloc((count + 1) * sizeof(char *))`**: aloca o array de
ponteiros. `count` tokens + 1 para o `NULL` final.

**`sizeof(char *)`**: o tamanho de um ponteiro. `malloc` precisa
do número de **bytes**, não de elementos.

```c
    size_t i = 0;
    const char *start = s;

    for (const char *p = s; ; p++) {
        if (*p == delim || *p == '\0') {
            size_t len = (size_t)(p - start);
            tokens[i] = malloc(len + 1);
            if (!tokens[i]) {
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
```

**O loop** percorre `s` com `p`. Quando `p` encontra um
delimitador (ou o `'\0'`), o trecho de `start` a `p` é um token.

**`size_t len = (size_t)(p - start)`**: a distância entre `p` e
`start`, em **elementos** (chars). Ver
[`docs/pointer-arithmetic.md`](../../docs/pointer-arithmetic.md).

**`tokens[i] = malloc(len + 1)`**: aloca `len` caracteres + 1
para o `'\0'`.

**`if (!tokens[i])`**: se a alocação falhar, **libera os tokens
já alocados** e o array, e retorna `NULL`. Isso evita memory
leak.

**`memcpy(tokens[i], start, len)`**: copia `len` bytes.

**`tokens[i][len] = '\0'`**: termina o token.

**`i++`**: avança para o próximo token.

**`if (*p == '\0') break`**: se chegou ao fim da string, para o
loop.

**`start = p + 1`**: o próximo token começa depois do
delimitador.

```c
    tokens[i] = NULL;
    return tokens;
}
```

**`tokens[i] = NULL`**: termina o array (como `argv`). O
chamador pode percorrer com `for (char **p = tokens; *p; p++)`.

**`return tokens`**: retorna o array. O chamador libera **cada
string** e o **array**.

### `strjoin`

```c
char *strjoin(char *const *parts, const char *sep)
{
    if (!parts)
        return NULL;
    if (!sep)
        sep = "";
```

**`char *const *parts`**: o array de ponteiros é `const` (não
modificamos os ponteiros), mas as strings podem ser modificadas
(embora não modifiquemos). Ver
[`docs/const-pointers.md`](../../docs/const-pointers.md).

**`if (!parts) return NULL`**: se `parts` for `NULL`, retorna
`NULL`.

**`if (!sep) sep = ""`**: se `sep` for `NULL`, trata como string
vazia.

```c
    size_t total = 0;
    size_t n_parts = 0;
    for (char *const *p = parts; *p; p++) {
        total += strlen(*p);
        n_parts++;
    }
```

**Primeira passada**: calcula o tamanho total (soma dos
`strlen` das partes) e o número de partes.

```c
    if (n_parts == 0)
        return strdup("");
```

**Se não há partes**: retorna uma string vazia alocada.

```c
    total += (n_parts - 1) * strlen(sep);
```

**Adiciona os separadores**: entre `n_parts` partes, há
`n_parts - 1` separadores.

```c
    char *joined = malloc(total + 1);
    if (!joined)
        return NULL;
```

**`malloc(total + 1)`**: `total` caracteres + 1 para o `'\0'`.

```c
    char *dst = joined;
    for (char *const *p = parts; *p; p++) {
        size_t len = strlen(*p);
        memcpy(dst, *p, len);
        dst += len;
        if (*(p + 1) != NULL) {
            size_t sep_len = strlen(sep);
            memcpy(dst, sep, sep_len);
            dst += sep_len;
        }
    }
    *dst = '\0';
```

**Segunda passada**: copia as partes e os separadores. O `dst`
avança a cada cópia.

**`if (*(p + 1) != NULL)`**: verifica se **não** é a última
parte (não coloca separador depois da última).

**`*dst = '\0'`**: termina a string.

```c
    return joined;
}
```

**`return joined`**: retorna a string juntada. O chamador libera
com `free`.

## `main.c` (a CLI)

### Includes

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strings.h"
```

**`<stdio.h>`**: para `printf`, `fprintf`, `fgets`.

**`<stdlib.h>`**: para `free`, `strtol` (não usado diretamente,
mas o `main` pode precisar).

**`<string.h>`**: para `strcmp`.

**`"strings.h"`**: o header do módulo.

### `print_help`

```c
static void print_help(const char *prog)
{
    printf(
        "Uso: %s COMANDO [ARGUMENTOS...]\n"
        ...
    );
}
```

**`static`**: helper interno.

**`const char *prog`**: o nome do programa (para o help).

### `main`

```c
int main(int argc, char **argv)
{
    if (argc < 2) {
        print_help(argv[0]);
        return 1;
    }

    const char *cmd = argv[1];
```

**`argc < 2`**: se não há comando, mostra o help e sai com erro
(1).

**`cmd`**: o comando (primeiro argumento).

```c
    if (strcmp(cmd, "dup") == 0) {
        if (argc != 3) {
            fprintf(stderr, "%s: dup requer uma string\n", argv[0]);
            return 1;
        }
        char *r = strdup(argv[2]);
        if (!r) {
            fprintf(stderr, "%s: erro de alocacao\n", argv[0]);
            return 1;
        }
        printf("%s\n", r);
        free(r);
    }
```

**`strcmp(cmd, "dup")`**: verifica se o comando é `dup`.

**`if (argc != 3)`**: `dup` precisa de **um** argumento (a
string). `argc` é 3 (`prog`, `dup`, string).

**`strdup(argv[2])`**: duplica a string.

**`printf("%s\n", r)`**: imprime o resultado.

**`free(r)`**: libera a memória.

**Os outros comandos** seguem o mesmo padrão (`ndup`, `trim`,
`rev`, `lower`, `upper`, `split`, `join`).

```c
    else if (strcmp(cmd, "split") == 0) {
        if (argc != 4) {
            fprintf(stderr, "%s: split requer string e delimitador\n",
                    argv[0]);
            return 1;
        }
        char delim = argv[3][0];
        char **tokens = strsplit(argv[2], delim);
        if (!tokens) {
            fprintf(stderr, "%s: erro de alocacao\n", argv[0]);
            return 1;
        }
        for (char **p = tokens; *p; p++)
            printf("%s\n", *p);
        for (char **p = tokens; *p; p++)
            free(*p);
        free(tokens);
    }
```

**`split`**: precisa de string e delimitador (2 argumentos).

**`char delim = argv[3][0]`**: o delimitador é o **primeiro**
caractere do terceiro argumento.

**`strsplit(argv[2], delim)`**: divide a string.

**`for (char **p = tokens; *p; p++) printf(...)`**: imprime cada
token.

**`for (char **p = tokens; *p; p++) free(*p)`**: libera cada
token.

**`free(tokens)`**: libera o array.

```c
    else {
        fprintf(stderr, "%s: comando desconhecido: '%s'\n",
                argv[0], cmd);
        fprintf(stderr, "Tente '%s help' para ajuda.\n", argv[0]);
        return 1;
    }

    return 0;
}
```

**Comando desconhecido**: erro + sugestão de `help`.

## Resumo do `main.c`

- **Parsing manual de comandos** (`dup`, `ndup`, `trim`, `rev`,
  `lower`, `upper`, `split`, `join`, `help`).
- **Cada comando** verifica o número de argumentos, chama a
  função, imprime o resultado, e libera a memória.
- **`split`** e **`join`** lidam com arrays de strings
  (`char **`).

---

arataca89@gmail.com

Aulas de programação