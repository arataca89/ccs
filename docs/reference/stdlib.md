# `<stdlib.h>` — utilidades gerais

Funções para alocação de memória, conversão de strings,
controle de processo, e outras utilidades. Usado em quase todos
os projetos do CCS.

## Alocação de memória

### `malloc`

**Header**: `<stdlib.h>`

**Assinatura**: `void *malloc(size_t size);`

**O que faz**: aloca **`size` bytes** de memória no heap, e
retorna um ponteiro para o início do bloco.

**Retorno**: ponteiro para o bloco alocado, ou `NULL` se a
alocação falhar (memória esgotada).

**Onde usamos**: `003` (`grow_buffer`, `read_line`), `004`
(`list_create`, `list_push`, `list_append`).

**Cuidados**: **sempre** verifique `NULL`. A memória **não é
inicializada** (contém lixo). Para inicializar com zeros, use
`calloc`. O bloco deve ser liberado com `free`.

**Ver também**: [`calloc`](#calloc), [`realloc`](#realloc),
[`free`](#free), [`size_t`](stddef.md#size_t).

### `calloc`

**Header**: `<stdlib.h>`

**Assinatura**: `void *calloc(size_t nmemb, size_t size);`

**O que faz**: aloca memória para um **array** de `nmemb`
elementos, cada um com `size` bytes, e **inicializa todos os
bytes com zero**.

**Retorno**: ponteiro para o bloco alocado e zerado, ou `NULL`
se a alocação falhar.

**Diferença de `malloc`**:

- `malloc(n)` aloca `n` bytes, **sem inicializar** (contém lixo).
- `calloc(nmemb, size)` aloca `nmemb * size` bytes, **inicializa
  com zero**.

**Quando usar**:

- **Arrays**: `calloc(n, sizeof(int))` aloca `n` inteiros,
  todos valendo 0. Mais seguro que `malloc(n * sizeof(int))`
  seguido de `memset(ptr, 0, n * sizeof(int))`.
- **Structs**: `calloc(1, sizeof(struct Node))` aloca uma
  struct, com todos os campos zerados. Útil para inicializar
  ponteiros com `NULL` (que é 0 na prática).
- **Strings**: `calloc(n + 1, 1)` aloca `n + 1` bytes, todos
  zero. A string já começa vazia (o primeiro byte é `'\0'`).

**Onde usamos**: **não usamos** ainda (mas é uma alternativa
segura ao `malloc` + `memset`).

**Cuidados**: **sempre** verifique `NULL`. `calloc` pode ser
**mais lento** que `malloc` (porque zera a memória), mas é mais
seguro. O bloco deve ser liberado com `free`.

**Exemplo**:

```c
/* Aloca um array de 10 inteiros, todos zero. */
int *v = calloc(10, sizeof(int));
if (!v) { /* erro de alocação */ }
/* ... usa v[0..9], todos valendo 0 ... */
free(v);
```

**Ver também**: [`malloc`](#malloc), [`realloc`](#realloc),
[`free`](#free), [`memset`](string.md#memset).

### `realloc`

**Header**: `<stdlib.h>`

**Assinatura**: `void *realloc(void *ptr, size_t size);`

**O que faz**: redimensiona o bloco apontado por `ptr` para
**`size` bytes**. Pode mover o bloco para outro endereço (se
não houver espaço no lugar).

**Retorno**: ponteiro para o bloco redimensionado, ou `NULL` se
falhar (e o bloco original **não** é liberado).

**Onde usamos**: `003` (`grow_buffer`).

**Cuidados**: **nunca** faça `ptr = realloc(ptr, ...)`. Se
`realloc` falhar, retorna `NULL`, e o `ptr` original é perdido
(memory leak). Use um ponteiro temporário:

```c
char *tmp = realloc(ptr, new_size);
if (!tmp)
    return -1;   /* ptr original ainda é válido */
ptr = tmp;
```

Ver [notes.md do 003_grep](../../projects/003_grep/notes.md#o-erro-clássico-de-realloc).

**Ver também**: [`malloc`](#malloc), [`calloc`](#calloc),
[`free`](#free).

### `free`

**Header**: `<stdlib.h>`

**Assinatura**: `void free(void *ptr);`

**O que faz**: libera o bloco de memória apontado por `ptr`
(que deve ter sido retornado por `malloc`, `calloc`, ou
`realloc`).

**Retorno**: nada.

**Onde usamos**: `003` (`grep_stream`), `004` (`list_free`,
`list_remove`).

**Cuidados**: **nunca** libere o mesmo bloco duas vezes
(double free — comportamento indefinido). **Nunca** libere um
ponteiro que não veio de `malloc`/`calloc`/`realloc`. Depois de
`free(ptr)`, o ponteiro fica **inválido** (use-after-free se
usar). Para evitar, faça `free(ptr); ptr = NULL;`.

**Ver também**: [`malloc`](#malloc), [`calloc`](#calloc),
[`realloc`](#realloc).

## Conversão de strings

### `strtol`

**Header**: `<stdlib.h>`

**Assinatura**: `long strtol(const char *nptr, char **endptr, int base);`

**O que faz**: converte a string `nptr` em um `long`, na base
`base` (10 para decimal, 16 para hex, 0 para auto-detectar).
`endptr` recebe um ponteiro para o primeiro caractere **não
consumido** pela conversão.

**Retorno**: o valor convertido. Se nenhum dígito for
encontrado, retorna 0 e `endptr == nptr`. Se o valor estourar
`long`, retorna `LONG_MAX`/`LONG_MIN` e seta `errno = ERANGE`.

**Onde usamos**: `004` (`parse_int`).

**Cuidados**: **sempre** verifique `endptr` para detectar erros:

- `*endptr != '\0'`: sobrou lixo depois do número (`"42abc"`).
- `endptr == nptr`: nada foi convertido (`"abc"`, `""`).

Ver [notes.md do 004](../../projects/004_linked_list/notes.md#parse_int-com-strtol-por-que-não-atoi).

**Ver também**: [`atoi`](#atoi) (e por que **não** usar),
[`errno`](errno.md#errno).

### `atoi`

**Header**: `<stdlib.h>`

**Assinatura**: `int atoi(const char *nptr);`

**O que faz**: converte a string `nptr` em um `int`.

**Retorno**: o valor convertido, ou 0 se nenhum dígito for
encontrado.

**Onde usamos**: **não usamos** (e **não devemos** usar).

**Cuidados**: `atoi` **não detecta erros**. `atoi("42abc")`
retorna 42 (ignora o lixo). `atoi("abc")` retorna 0
(indistinguível de `atoi("0")`). **Sempre** use `strtol` em vez
de `atoi`.

**Ver também**: [`strtol`](#strtol).

### `getenv`

**Header**: `<stdlib.h>`

**Assinatura**: `char *getenv(const char *name);`

**O que faz**: retorna o valor da variável de ambiente `name`
(ex.: `getenv("PATH")`), ou `NULL` se não existir.

**Retorno**: ponteiro para o valor (na memória do ambiente), ou
`NULL`.

**Onde usamos**: **não usamos** ainda (mencionado em
[argc-argv.md](../argc-argv.md#o-envp-o-terceiro-argumento-opcional)).

**Cuidados**: o ponteiro retornado **não deve** ser liberado com
`free` (a memória é do ambiente). E **não deve** ser modificado
(use uma cópia, se precisar).

## Controle de processo

### `exit`

**Header**: `<stdlib.h>`

**Assinatura**: `void exit(int status);`

**O que faz**: termina o programa **imediatamente**, com o código
de retorno `status`. Descarrega os buffers de `stdio`, e executa
as funções registradas com `atexit` (não usamos `atexit`).

**Retorno**: não retorna (o programa termina).

**Onde usamos**: `002` (`-h` no `cat`), `005` (`-h` no `demo`).

**Cuidados**: `exit` é **legítimo** em casos terminais (`-h`,
erro fatal), mas **não** em casos recuperáveis (prefira `return`
e deixe o `main` decidir). Ver
[notes.md do 002_cat](../../projects/002_cat/notes.md#por-que--h-usa-exit0-e-o-default-usa-return--1).

**Ver também**: [`EXIT_SUCCESS`](#exit_success),
[`EXIT_FAILURE`](#exit_failure).

### `EXIT_SUCCESS`

**Header**: `<stdlib.h>`

**O que é**: uma macro que expande para **0** (ou para o código
de sucesso da plataforma, mas na prática é 0). Usada como
argumento de `exit` ou como retorno do `main`.

**Onde usamos**: não usamos explicitamente (usamos `return 0`).

**Cuidados**: `EXIT_SUCCESS` é **portável**, mas `0` também é
(na prática, em todas as plataformas relevantes). O C padrão
garante que `EXIT_SUCCESS` é 0 ou um valor que o sistema
considera sucesso.

**Ver também**: [`exit`](#exit), [`EXIT_FAILURE`](#exit_failure).

### `EXIT_FAILURE`

**Header**: `<stdlib.h>`

**O que é**: uma macro que expande para um valor **não-zero**
(tipicamente 1), indicando falha. Usada como argumento de `exit`
ou como retorno do `main`.

**Onde usamos**: não usamos explicitamente (usamos `return 1`).

**Ver também**: [`exit`](#exit), [`EXIT_SUCCESS`](#exit_success).

## Outras funções (não usadas ainda, mas úteis)

### `qsort`

**Header**: `<stdlib.h>`

**Assinatura**: `void qsort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *));`

**O que faz**: ordena um array de `nmemb` elementos de `size`
bytes cada, usando a função de comparação `compar`.

**Onde usamos**: **não usamos** ainda (será usado em projetos
futuros, como o `ls`).

**Cuidados**: a função `compar` recebe **ponteiros para os
elementos** (não os elementos em si). Para `int`, é
`int compar(const void *a, const void *b) { return *(int *)a -
*(int *)b; }`. Cuidado com **overflow** na subtração.

### `bsearch`

**Header**: `<stdlib.h>`

**Assinatura**: `void *bsearch(const void *key, const void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *));`

**O que faz**: busca `key` em um array **ordenado** de `nmemb`
elementos, usando `compar`.

**Retorno**: ponteiro para o elemento encontrado, ou `NULL`.

**Onde usamos**: **não usamos** ainda.

**Cuidados**: o array **precisa** estar ordenado (pelo mesmo
critério de `compar`). Senão, o resultado é indefinido.

### `rand`

**Header**: `<stdlib.h>`

**Assinatura**: `int rand(void);`

**O que faz**: retorna um número pseudo-aleatório entre 0 e
`RAND_MAX` (tipicamente 32767).

**Onde usamos**: **não usamos** ainda (será usado em jogos,
como o `snake`).

**Cuidados**: `rand` **não** é criptograficamente seguro, e a
sequência é **determinística** (a mesma seed dá a mesma
sequência). Para variar, use `srand(time(NULL))` no início do
programa. Para um número entre 0 e N-1, use `rand() % N`.

### `srand`

**Header**: `<stdlib.h>`

**Assinatura**: `void srand(unsigned int seed);`

**O que faz**: inicializa o gerador de números aleatórios com
`seed`.

**Onde usamos**: **não usamos** ainda.

**Cuidados**: chame `srand` **uma vez** no início do programa,
com uma seed variável (ex.: `time(NULL)`). Se não chamar, `rand`
usa seed 1 (sequência sempre igual).

### `abs`

**Header**: `<stdlib.h>`

**Assinatura**: `int abs(int j);`

**O que faz**: retorna o valor absoluto de `j`.

**Onde usamos**: **não usamos** ainda.

**Cuidados**: `abs(INT_MIN)` é **comportamento indefinido**
(porque `-INT_MIN` não cabe em `int`). Para `long`, use `labs`;
para `long long`, `llabs`.
