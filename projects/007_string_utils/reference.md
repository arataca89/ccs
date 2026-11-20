# `string_utils`  - Referência 

Referência das funções oferecidas pelo `string_utils`. Para cada
função: **assinatura**, **o que faz**, **retorno**, **quem
libera a memória**, **exemplo**, e **cuidados**.

## Strings mutáveis vs. imutáveis

Antes de usar as funções, é importante entender que **nem toda
string em C é mutável**. Há três casos:

### 1. String literal (`"ola"`) — imutável

```c
char *p = "ola";   /* p aponta para uma string LITERAL */
p[0] = 'X';        /* ⚠️ COMPORTAMENTO INDEFINIDO (pode dar segfault) */
```

**O que é**: uma string literal é armazenada em uma região de
memória **read-only** (somente leitura) do programa. Modificá-la
é **comportamento indefinido** (o programa pode crashar, ou pior,
"funcionar" e dar bug depois).

**Por que compila**: o tipo `char *` não impede a modificação. O
compilador pode até emitir um warning (com `-Wwrite-strings`),
mas **não** é erro. O C confia no programador.

**Como evitar**: use `const char *` para strings literais:

```c
const char *p = "ola";
p[0] = 'X';   /* ❌ ERRO de compilação (bom!) */
```

### 2. Array de char (`char s[] = "ola"`) — mutável

```c
char s[] = "ola";   /* s é um ARRAY, com cópia da string */
s[0] = 'X';         /* ✅ OK: s é mutável */
/* s = "Xla" */
```

**O que é**: `char s[] = "ola"` cria um **array** de 4 chars
(`'o'`, `'l'`, `'a'`, `'\0'`), e **copia** a string literal para
ele. O array está na **pilha** (ou no segmento de dados, se for
global), e é **mutável**.

**Por que funciona**: o array é uma **cópia** da string literal.
Modificar o array não afeta a literal original.

### 3. Ponteiro para memória alocada (`malloc`) — mutável

```c
char *p = malloc(4);
if (!p) { /* erro */ }
strcpy(p, "ola");   /* copia "ola" para a memória alocada */
p[0] = 'X';         /* ✅ OK: p aponta para memória mutável */
/* p = "Xla" */
free(p);
```

**O que é**: `malloc` aloca memória no **heap**, que é
**mutável**. O ponteiro `p` aponta para essa memória.

**Por que funciona**: o heap é mutável, e `p` aponta para lá.
Modificar `p[0]` modifica a memória alocada.

### Resumo

| Forma | Mutável? | Onde fica | Exemplo |
|---|---|---|---|
| `char *p = "ola"` | **Não** (o conteúdo) | Read-only | `p[0] = 'X'` é **erro** |
| `const char *p = "ola"` | **Não** (o conteúdo) | Read-only | `p[0] = 'X'` é **erro de compilação** |
| `char s[] = "ola"` | **Sim** | Pilha/dados | `s[0] = 'X'` é **OK** |
| `char *p = malloc(...)` | **Sim** | Heap | `p[0] = 'X'` é **OK** |
| `const char *p = malloc(...)` | **Não** (o conteúdo) | Heap | `p[0] = 'X'` é **erro de compilação** |

### Por que isso importa para o `string_utils`

- **`strrev`**, **`strtolower`**, **`strtoupper`**: modificam a
  string **in-place**. O chamador **precisa** passar uma string
  **mutável** (array ou `malloc`). Se passar uma literal, é
  **comportamento indefinido**.
- **`strdup`**, **`strndup`**, **`strtrim`**, **`strsplit`**,
  **`strjoin`**: **alocam** uma nova string (mutável). O
  chamador pode modificar o resultado.

**Regra prática**: se a função modifica a string (in-place), o
chamador precisa garantir que ela é mutável. Na dúvida, faça uma
cópia com `strdup` antes.

## Índice

- [`strdup`](#strdup)
- [`strndup`](#strndup)
- [`strtrim`](#strtrim)
- [`strrev`](#strrev)
- [`strtolower`](#strtolower)
- [`strtoupper`](#strtoupper)
- [`strsplit`](#strsplit)
- [`strjoin`](#strjoin)

## `strdup`

**Assinatura**: `char *strdup(const char *s);`

**O que faz**: duplica a string `s` (aloca memória com `malloc`,
copia `s` incluindo o `'\0'`).

**Retorno**: ponteiro para a cópia, ou `NULL` se a alocação falhar.

**Quem libera**: o **chamador**, com `free`.

**Exemplo**:

```c
char *copia = strdup("ola");
if (!copia) {
    /* erro de alocação */
}
printf("%s\n", copia);   /* ola */
free(copia);
```

**Cuidados**:

- **Sempre** verifique `NULL` (a alocação pode falhar).
- **Sempre** libere com `free` (senão, memory leak).
- A cópia é **independente** do original (modificar uma não afeta
  a outra).

**Equivalente POSIX**: `strdup` (em `<string.h>`, com
`_POSIX_C_SOURCE`). No Windows/MSVC, é `_strdup`.

## `strndup`

**Assinatura**: `char *strndup(const char *s, size_t n);`

**O que faz**: duplica **até `n` caracteres** de `s`. Se `s` for
menor que `n`, duplica a string inteira (e o `'\0'`).

**Retorno**: ponteiro para a cópia, ou `NULL`.

**Quem libera**: o chamador, com `free`.

**Exemplo**:

```c
char *copia = strndup("ola mundo", 3);
/* copia = "ola" */
free(copia);
```

**Cuidados**:

- **Sempre** verifique `NULL`.
- **Sempre** libere com `free`.
- A cópia é **sempre** terminada em `'\0'`, mesmo que `s` não
  seja (se `n` for menor que o tamanho de `s`, a cópia tem `n`
  caracteres + `'\0'`).
- `n` é `size_t` (não `int`).

**Equivalente POSIX**: `strndup` (em `<string.h>`, com
`_POSIX_C_SOURCE`). No Windows/MSVC, é `_strndup`.

## `strtrim`

**Assinatura**: `char *strtrim(const char *s);`

**O que faz**: remove **espaços** (espaço, `\t`, `\n`, `\r`,
`\v`, `\f`) do **início** e do **fim** de `s`, retornando uma
**nova string alocada**.

**Retorno**: ponteiro para a string "trimada", ou `NULL`.

**Quem libera**: o chamador, com `free`.

**Exemplo**:

```c
char *trimada = strtrim("  ola  ");
/* trimada = "ola" */
free(trimada);
```

**Cuidados**:

- **Sempre** verifique `NULL`.
- **Sempre** libere com `free`.
- **Não modifica** `s` (retorna uma nova string).
- Se `s` for só espaços, retorna uma string vazia (`""`).
- Se `s` for `NULL`, retorna `NULL` (ou uma string vazia? Ver
  `notes.md` para a decisão).
- A aritmética de ponteiros (`end - start + 1`) retorna
  `ptrdiff_t` (signed), que é convertido para `size_t` (com cast
  explícito). Ver
  [`docs/reference/stddef.md#ptrdiff_t`](../../docs/reference/stddef.md#ptrdiff_t)
  e
  [`docs/pointer-arithmetic.md`](../../docs/pointer-arithmetic.md#o-tipo-ptrdiff_t).

## `strrev`

**Assinatura**: `void strrev(char *s);`

**O que faz**: inverte a string `s` **in-place** (modifica `s`).

**Retorno**: nada.

**Quem libera**: ninguém (não aloca; modifica `s`).

**Exemplo**:

```c
char s[] = "ola";
strrev(s);
/* s = "alo" */
```

**Cuidados**:

- **`s` precisa ser mutável** (array ou `malloc`). **Não** pode
  ser literal (`char *s = "ola"; strrev(s);` é **erro** —
  comportamento indefinido).
- **Não aloca** memória. Se você quer uma cópia invertida, faça
  `strdup` antes.
- Se `s` for `NULL` ou vazia, não faz nada.

## `strtolower`

**Assinatura**: `void strtolower(char *s);`

**O que faz**: converte todos os caracteres **maiúsculos** de `s`
para **minúsculos**, in-place.

**Retorno**: nada.

**Quem libera**: ninguém (não aloca; modifica `s`).

**Exemplo**:

```c
char s[] = "Ola Mundo";
strtolower(s);
/* s = "ola mundo" */
```

**Cuidados**:

- **`s` precisa ser mutável**.
- **Não aloca**.
- Usa `tolower((unsigned char)c)` internamente (portabilidade).
- Se `s` for `NULL`, não faz nada.

## `strtoupper`

**Assinatura**: `void strtoupper(char *s);`

**O que faz**: converte todos os caracteres **minúsculos** de `s`
para **maiúsculos**, in-place.

**Retorno**: nada.

**Quem libera**: ninguém (não aloca; modifica `s`).

**Exemplo**:

```c
char s[] = "Ola Mundo";
strtoupper(s);
/* s = "OLA MUNDO" */
```

**Cuidados**:

- **`s` precisa ser mutável**.
- **Não aloca**.
- Usa `toupper((unsigned char)c)` internamente.
- Se `s` for `NULL`, não faz nada.

## `strsplit`

**Assinatura**: `char **strsplit(const char *s, char delim);`

**O que faz**: divide `s` pelo delimitador `delim`, retornando
um **array de strings** terminado em `NULL`.

**Retorno**: array de strings (`char **`), ou `NULL` em erro.

**Quem libera**: o chamador, com `free` em **cada string** e no
**array**:

```c
for (char **p = tokens; *p; p++)
    free(*p);
free(tokens);
```

**Exemplo**:

```c
char **tokens = strsplit("a,b,c", ',');
/* tokens[0] = "a", tokens[1] = "b", tokens[2] = "c", tokens[3] = NULL */
for (char **p = tokens; *p; p++)
    printf("%s\n", *p);
/* imprime: a, b, c */
for (char **p = tokens; *p; p++)
    free(*p);
free(tokens);
```

**Cuidados**:

- **Sempre** verifique `NULL`.
- **Sempre** libere **cada string** e o **array**.
- O array é terminado em `NULL` (como `argv`).
- Se `s` for vazia (`""`), retorna um array com **um** elemento
  (`""`) ou **zero** elementos? Ver `notes.md` para a decisão.
- Se `s` for `NULL`, retorna `NULL`.
- Delimitadores consecutivos (`"a,,b"`) geram string vazia no
  meio? Ver `notes.md` para a decisão.

## `strjoin`

**Assinatura**: `char *strjoin(char *const *parts, const char *sep);`

**O que faz**: junta as strings de `parts` (terminado em `NULL`)
com o separador `sep`, retornando uma **nova string alocada**.

**Retorno**: ponteiro para a string juntada, ou `NULL`.

**Quem libera**: o chamador, com `free`.

**Exemplo**:

```c
char *parts[] = { "a", "b", "c", NULL };
char *joined = strjoin(parts, ", ");
/* joined = "a, b, c" */
free(joined);
```

**Cuidados**:

- **Sempre** verifique `NULL`.
- **Sempre** libere com `free`.
- Se `parts` for vazio (`NULL` no primeiro elemento), retorna
  uma string vazia (`""`).
- Se `parts` for `NULL`, retorna `NULL`.
- `sep` pode ser `NULL`? Ver `notes.md` para a decisão.
- O array `parts` **não** é modificado (é `const`).

## Convenções gerais

- **Funções que alocam** (`strdup`, `strndup`, `strtrim`,
  `strsplit`, `strjoin`): retornam `NULL` em erro; o **chamador**
  libera com `free`.
- **Funções in-place** (`strrev`, `strtolower`, `strtoupper`):
  não alocam; modificam a string passada. O chamador precisa
  garantir que a string é **mutável**.
- **`NULL` como entrada**: a maioria das funções retorna `NULL`
  ou não faz nada (ver cada ficha).
- **`size_t`** para tamanhos (não `int`).
- **`const`** em parâmetros que a função não modifica.

## Como compilar

O `string_utils` é uma **biblioteca** (não um executável). Para
usar em outro projeto:

1. Copie `include/strings.h` e `src/strings.c` para o seu
   projeto.
2. Adicione `src/strings.c` ao seu `CMakeLists.txt`.
3. Inclua `#include "strings.h"` no seu código.

Ou, se quiser compilar o **demo** (o `main.c` do
`007_string_utils`):

    cmake -S . -B build -G "MinGW Makefiles"
    cmake --build build
    build\strings.exe -h

---

arataca89@gmail.com

Aulas de programação
