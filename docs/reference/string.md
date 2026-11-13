# `<string.h>` — strings e memória

Funções para manipular strings (sequências de `char` terminadas
em `'\0'`) e blocos de memória. Usado em quase todos os
projetos do CCS.

## Strings

### `strlen`

**Header**: `<string.h>`

**Assinatura**: `size_t strlen(const char *s);`

**O que faz**: retorna o **comprimento** da string `s` (o número
de caracteres **antes** do `'\0'`).

**Retorno**: o comprimento (sem contar o `'\0'`).

**Onde usamos**: `003` (`strip_newline`), `004`
(`strip_newline`), `005` (não diretamente).

**Cuidados**: `strlen` **percorre** a string até o `'\0'`. Se a
string não for terminada em `'\0'`, o comportamento é
**indefinido** (pode ler memória além do buffer). O retorno é
`size_t` (unsigned), não `int`.

**Ver também**: [`size_t`](stddef.md#size_t).

### `strcmp`

**Header**: `<string.h>`

**Assinatura**: `int strcmp(const char *s1, const char *s2);`

**O que faz**: compara `s1` e `s2` **caractere por caractere**
(até o `'\0'`).

**Retorno**:
- `< 0`: `s1` é **menor** que `s2` (primeiro caractere diferente
  é menor em `s1`).
- `0`: `s1` é **igual** a `s2`.
- `> 0`: `s1` é **maior** que `s2`.

**Onde usamos**: `002` (`parse_flags`), `003` (`parse_flags`,
`process_one`), `004` (`main`), `005` (`getopt`).

**Cuidados**: **não** use `==` para comparar strings
(`s1 == s2` compara **ponteiros**, não conteúdo). **Sempre** use
`strcmp`. A comparação é **case-sensitive** (`"abc"` ≠ `"ABC"`).

**Ver também**: [`strncmp`](#strncmp).

### `strncmp`

**Header**: `<string.h>`

**Assinatura**: `int strncmp(const char *s1, const char *s2, size_t n);`

**O que faz**: como `strcmp`, mas compara **no máximo `n`
caracteres**.

**Retorno**: mesmo que `strcmp`.

**Onde usamos**: **não usamos** ainda.

**Cuidados**: útil para comparar **prefixos** (`strncmp(s, "abc",
3) == 0` verifica se `s` começa com `"abc"`).

**Ver também**: [`strcmp`](#strcmp).

### `strcpy`

**Header**: `<string.h>`

**Assinatura**: `char *strcpy(char *dest, const char *src);`

**O que faz**: copia a string `src` (incluindo o `'\0'`) para
`dest`.

**Retorno**: `dest`.

**Onde usamos**: **não usamos** ainda.

**Cuidados**: **não verifica o tamanho** de `dest`. Se `src` for
maior que `dest`, **buffer overflow** (comportamento indefinido).
Prefira `strncpy` ou `snprintf` (com limite de tamanho).

**Ver também**: [`strncpy`](#strncpy), [`memcpy`](#memcpy).

### `strncpy`

**Header**: `<string.h>`

**Assinatura**: `char *strncpy(char *dest, const char *src, size_t n);`

**O que faz**: copia **no máximo `n`** caracteres de `src` para
`dest`. Se `src` for menor que `n`, **preenche o resto** com
`'\0'`.

**Retorno**: `dest`.

**Onde usamos**: **não usamos** ainda.

**Cuidados**: **`strncpy` é uma armadilha**:

1. **Não garante `'\0'`**: se `src` tem `n` ou mais caracteres,
   `dest` **não** é terminado em `'\0'`. Você precisa fazer
   `dest[n-1] = '\0'` manualmente.
2. **Preenche com `'\0'`**: se `src` é menor que `n`, preenche o
   resto com `'\0'` (pode ser lento para `n` grande).

**Alternativa**: `snprintf(dest, n, "%s", src)` é mais seguro
(garante `'\0'` e não preenche).

**Ver também**: [`strcpy`](#strcpy), [`snprintf`](stdio.md#printf)
(na verdade, `snprintf` não tem ficha ainda; fica para depois).

### `strcat`

**Header**: `<string.h>`

**Assinatura**: `char *strcat(char *dest, const char *src);`

**O que faz**: concatena `src` ao final de `dest` (o `'\0'` de
`dest` é substituído pelo primeiro caractere de `src`).

**Retorno**: `dest`.

**Onde usamos**: **não usamos** ainda.

**Cuidados**: **não verifica o tamanho** de `dest`. Se `dest` não
tiver espaço para `src`, **buffer overflow**. Prefira `strncat`
(com limite) ou `snprintf`.

**Ver também**: [`strncat`](#strncat) (não tem ficha ainda).

### `strchr`

**Header**: `<string.h>`

**Assinatura**: `char *strchr(const char *s, int c);`

**O que faz**: procura a **primeira ocorrência** do caractere `c`
em `s`.

**Retorno**: ponteiro para o caractere encontrado, ou `NULL` se
não encontrar.

**Onde usamos**: `004` (`main`, para separar comando e
argumento).

**Cuidados**: o `c` é `int` (não `char`), porque `strchr`
aceita `EOF` (que é negativo). Na prática, você passa um `char`
(convertido para `int`).

**Ver também**: [`strrchr`](#strrchr).

### `strrchr`

**Header**: `<string.h>`

**Assinatura**: `char *strrchr(const char *s, int c);`

**O que faz**: procura a **última ocorrência** do caractere `c`
em `s`.

**Retorno**: ponteiro para o caractere encontrado, ou `NULL`.

**Onde usamos**: **não usamos** ainda.

**Cuidados**: útil para achar a **extensão** de um arquivo
(`strrchr(nome, '.')`).

**Ver também**: [`strchr`](#strchr).

### `strstr`

**Header**: `<string.h>`

**Assinatura**: `char *strstr(const char *haystack, const char *needle);`

**O que faz**: procura a **primeira ocorrência** da substring
`needle` em `haystack`.

**Retorno**: ponteiro para o início da substring encontrada, ou
`NULL`.

**Onde usamos**: `003` (`line_matches`).

**Cuidados**: se `needle` for `""` (string vazia), retorna
`haystack` (a string inteira). A busca é **case-sensitive**. Para
case-insensitive, use `strcasestr` (não é C padrão; o `003_grep`
implementa a sua).

**Ver também**: [`strchr`](#strchr).

## Memória

### `memcpy`

**Header**: `<string.h>`

**Assinatura**: `void *memcpy(void *dest, const void *src, size_t n);`

**O que faz**: copia **`n` bytes** de `src` para `dest`.

**Retorno**: `dest`.

**Onde usamos**: **não usamos** ainda (mas é a base de `strdup`).

**Cuidados**: **não** funciona se `src` e `dest` **se
sobrepõem** (use `memmove` nesse caso). Não verifica tamanho
(buffer overflow se `n` for maior que `dest`).

**Ver também**: [`memmove`](#memmove), [`memset`](#memset).

### `memmove`

**Header**: `<string.h>`

**Assinatura**: `void *memmove(void *dest, const void *src, size_t n);`

**O que faz**: como `memcpy`, mas **funciona** se `src` e `dest`
se sobrepõem (copia em uma ordem segura).

**Retorno**: `dest`.

**Onde usamos**: **não usamos** ainda.

**Cuidados**: é **mais lento** que `memcpy` (porque verifica
sobreposição). Use `memcpy` quando souber que não há sobreposição.

**Ver também**: [`memcpy`](#memcpy).

### `memset`

**Header**: `<string.h>`

**Assinatura**: `void *memset(void *s, int c, size_t n);`

**O que faz**: preenche **`n` bytes** de `s` com o valor `c`
(convertido para `unsigned char`).

**Retorno**: `s`.

**Onde usamos**: **não usamos** ainda (mas `calloc` é uma
alternativa para zerar memória).

**Cuidados**: `memset(s, 0, n)` zera `n` bytes. Mas
`memset(s, 1, n)` **não** preenche com o valor 1 (int); preenche
cada **byte** com `0x01`, o que em `int` (4 bytes) vira
`0x01010101` (16843009), não 1. Use `memset` só para **bytes**
(`char`, `unsigned char`), não para `int`/`float`.

**Ver também**: [`calloc`](stdlib.md#calloc), [`memcpy`](#memcpy).

### `memcmp`

**Header**: `<string.h>`

**Assinatura**: `int memcmp(const void *s1, const void *s2, size_t n);`

**O que faz**: compara **`n` bytes** de `s1` e `s2`.

**Retorno**: mesmo que `strcmp` (`< 0`, `0`, `> 0`).

**Onde usamos**: **não usamos** ainda.

**Cuidados**: útil para comparar **structs** ou **buffers
binários** (onde `strcmp` não funciona, porque pode ter `'\0'` no
meio).

**Ver também**: [`strcmp`](#strcmp).

## Outras funções (não usadas ainda, mas úteis)

### `strdup`

**Header**: `<string.h>` (POSIX; no Windows/MSVC, é `_strdup`
em `<string.h>`)

**Assinatura**: `char *strdup(const char *s);`

**O que faz**: aloca memória com `malloc`, copia `s` (incluindo
o `'\0'`), e retorna o novo ponteiro.

**Retorno**: ponteiro para a cópia, ou `NULL` se a alocação
falhar.

**Onde usamos**: **não usamos** ainda (mas é a base de qualquer
programa que precisa de uma cópia modificável de uma string).

**Cuidados**: `strdup` **não é C padrão** (é POSIX). No
Windows/MSVC, é `_strdup`. O chamador **deve** liberar o
resultado com `free`.

**Implementação** (para entender):

```c
char *strdup(const char *s)
{
    size_t len = strlen(s) + 1;
    char *copy = malloc(len);
    if (!copy)
        return NULL;
    memcpy(copy, s, len);
    return copy;
}
```

**Ver também**: [`malloc`](stdlib.md#malloc), [`free`](stdlib.md#free),
[`strlen`](#strlen), [`memcpy`](#memcpy).

### `strtok`

**Header**: `<string.h>`

**Assinatura**: `char *strtok(char *str, const char *delim);`

**O que faz**: divide `str` em **tokens**, usando os caracteres
de `delim` como separadores. Na primeira chamada, passe `str`;
nas seguintes, passe `NULL` (para continuar de onde parou).

**Retorno**: ponteiro para o próximo token, ou `NULL` quando
acabarem.

**Onde usamos**: **não usamos** ainda (será usado em parsers,
como o `expression_parser`).

**Cuidados**: `strtok` **modifica** `str` (substitui os
separadores por `'\0'`). E **não é thread-safe** (usa estado
`static` interno). Para uma versão thread-safe, use `strtok_r`
(POSIX).

**Ver também**: [`strchr`](#strchr).
