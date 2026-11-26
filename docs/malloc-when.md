# Quando usar `malloc`

> `malloc` aloca memória no heap. Mas **quando** usar? Muitos
> alunos alocam demais (ou de menos). Este texto explica os
> critérios, com exemplos do CCS.

## O que é `malloc`

`malloc` (memory allocation) aloca um bloco de memória no **heap**,
e retorna um ponteiro para o início do bloco.

```c
void *malloc(size_t size);
```

**Retorno**: ponteiro para o bloco, ou `NULL` se a alocação falhar
(memória esgotada).

**Quem libera**: o **chamador**, com `free`.

**A memória não é inicializada** (contém lixo). Para inicializar
com zeros, use `calloc`.

## Quando usar `malloc`

### 1. Quando o tamanho não é conhecido em tempo de compilação

**Exemplo**: o `003_grep` lê linhas de um arquivo. O tamanho da
linha **não é conhecido** até a leitura. O `read_line` aloca o
buffer dinamicamente, e cresce com `realloc`:

```c
char *buf = NULL;
size_t cap = 0;
while ((len = read_line(in, &buf, &cap, ...)) >= 0) {
    /* buf tem a linha (alocada dinamicamente) */
}
free(buf);
```

**Por que `malloc` e não array fixo**: um array fixo (ex.: `char
buf[256]`) **truncaria** linhas maiores que 256. O `malloc` cresce
conforme a necessidade.

### 2. Quando o tamanho é grande (e não cabe na pilha)

**Exemplo**: o `004_linked_list` tem uma lista que pode ter muitos
nós. Cada nó é alocado com `malloc`:

```c
struct Node *n = malloc(sizeof *n);
```

**Por que `malloc` e não array fixo**: a lista pode ter **qualquer**
tamanho (depende do que o usuário insere). Um array fixo teria um
limite.

**A pilha é limitada**: a pilha (stack) tem um tamanho fixo
(tipicamente 1–8 MB). Se você alocar um array grande na pilha
(ex.: `int v[1000000]`), pode dar **stack overflow**. O `malloc`
usa o heap, que é maior.

### 3. Quando o dado precisa "sobreviver" à função

**Exemplo**: o `007_string_utils` tem `strdup`, que duplica uma
string. A cópia é alocada com `malloc`, e **retornada** para o
chamador:

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

**Por que `malloc` e não array local**: um array local (na pilha)
é **destruído** quando a função retorna. Se você retornar um
ponteiro para ele, o chamador terá um **ponteiro inválido**
(dangling pointer). O `malloc` aloca no heap, que **persiste** até
o `free`.

### 4. Quando o número de elementos é dinâmico

**Exemplo**: o `009_snake` tem a cobra, que cresce e encolhe. O
`body` da cobra é alocado com `malloc` (no `game_init`), com
`capacity = target + 1`:

```c
game->snake.capacity = target + 1;
game->snake.body = malloc((size_t)game->snake.capacity * sizeof(Point));
```

**Por que `malloc` e não array fixo**: o tamanho da cobra depende
do **alvo** (que é configurável). Um array fixo seria `size *
size` (o tabuleiro inteiro), que é **desperdício** (a cobra nunca
ocupa tudo).

## Quando **não** usar `malloc`

### 1. Quando o tamanho é pequeno e conhecido

**Exemplo**: um buffer para ler uma linha de comando (curta). O
`004_linked_list` usa `char line[256]` (array fixo) para ler os
comandos:

```c
char line[LINE_MAX];   // #define LINE_MAX 256
while (fgets(line, sizeof line, stdin) != NULL) { ... }
```

**Por que não `malloc`**: os comandos são curtos (ex.: `push 42`,
`print`). 256 bytes é **muito** mais que suficiente. Alocar
dinamicamente seria **overkill** (mais código, mais chance de
erro).

### 2. Quando o dado é local e temporário

**Exemplo**: uma variável para guardar um resultado intermediário.
Não precisa de `malloc` — a pilha é suficiente:

```c
int soma = 0;
for (int i = 0; i < n; i++)
    soma += v[i];
```

**Por que não `malloc`**: a variável `soma` é local, e **não
precisa** sobreviver à função. A pilha é mais rápida e mais
segura (o `free` é automático).

### 3. Quando o compilador pode calcular o tamanho

**Exemplo**: um array cujo tamanho é uma constante de compilação:

```c
int v[10];   // 10 é constante
```

**Por que não `malloc`**: o compilador sabe o tamanho, e aloca na
pilha (ou no segmento de dados). O `malloc` seria **desnecessário**.

### 4. Quando você pode usar VLA (Variable Length Array)

**Exemplo**: um array cujo tamanho é uma **variável** (mas não
muito grande):

```c
int n = 100;
int v[n];   // VLA (C99)
```

**Por que não `malloc`**: o VLA aloca na pilha automaticamente.
**Mas**: o VLA tem riscos (stack overflow se `n` for grande), e
**não é C11** (é opcional em C11, e removido em C23). **No CCS**,
evitamos VLA (usamos `malloc` quando o tamanho é variável).

## O erro clássico: `malloc` sem `free`

**O problema**: se você aloca e **esquece** de liberar, a memória
fica ocupada até o programa terminar. Em programas de longa
duração (servidores, bibliotecas), isso é **memory leak** (o
programa consome cada vez mais memória).

**Exemplo** (errado):

```c
char *copy = strdup("ola");
printf("%s\n", copy);
/* esqueceu o free(copy) */
```

**Exemplo** (correto):

```c
char *copy = strdup("ola");
if (!copy) { /* erro */ }
printf("%s\n", copy);
free(copy);
```

**No CCS**: todos os projetos que usam `malloc` têm o `free`
correspondente (no `game_free`, no `list_free`, etc.).

## O erro clássico: `free` sem `malloc`

**O problema**: se você chama `free` em um ponteiro que **não**
veio de `malloc` (ou que já foi liberado), é **comportamento
indefinido** (pode crashar, ou pior, corromper a memória).

**Exemplo** (errado):

```c
char *p = "ola";
free(p);   // ERRADO: p não veio de malloc
```

**Exemplo** (errado):

```c
char *p = malloc(10);
free(p);
free(p);   // ERRADO: double free
```

**No CCS**: o `game_free` libera o `body` (que veio de `malloc` no
`game_init`), e **só uma vez**.

## O erro clássico: `realloc` sem ponteiro temporário

**O problema**: se `realloc` falhar, retorna `NULL`, e o ponteiro
original é **perdido** (memory leak).

**Exemplo** (errado):

```c
buf = realloc(buf, new_size);
if (!buf) { /* erro, mas o buf original foi perdido */ }
```

**Exemplo** (correto):

```c
char *tmp = realloc(buf, new_size);
if (!tmp) { /* erro, mas o buf original ainda é válido */ }
buf = tmp;
```

**No CCS**: o `grow_buffer` (do `003_grep`) usa o ponteiro
temporário.

## O erro clássico: `malloc` sem verificar `NULL`

**O problema**: se `malloc` falhar, retorna `NULL`. Se você usar o
ponteiro sem verificar, é **comportamento indefinido** (segfault).

**Exemplo** (errado):

```c
struct Node *n = malloc(sizeof *n);
n->value = 42;   // ERRADO: n pode ser NULL
```

**Exemplo** (correto):

```c
struct Node *n = malloc(sizeof *n);
if (!n)
    return -1;
n->value = 42;
```

**No CCS**: todos os `malloc` são verificados.

## Resumo

| Quando usar `malloc` | Quando **não** usar |
|---|---|
| Tamanho desconhecido em tempo de compilação | Tamanho pequeno e conhecido |
| Tamanho grande (não cabe na pilha) | Dado local e temporário |
| Dado precisa sobreviver à função | Tamanho é constante de compilação |
| Número de elementos é dinâmico | Pode usar VLA (mas cuidado) |

**Regras**:

1. **Sempre** verifique `NULL` após `malloc`.
2. **Sempre** libere com `free` (uma vez).
3. **Nunca** libere ponteiro que não veio de `malloc`.
4. **Nunca** libere o mesmo bloco duas vezes (double free).
5. **Use `realloc` com ponteiro temporário**.
6. **`malloc` não inicializa** a memória (use `calloc` para
   inicializar com zeros).
7. **`malloc` é mais lento** que a pilha (evite para dados
   pequenos).

## Onde o `malloc` aparece no CCS

- **`003_grep`**: `read_line` (buffer de linha), `grow_buffer`
  (cresce o buffer).
- **`004_linked_list`**: `list_create` (a lista), `list_push`,
  `list_append` (os nós).
- **`007_string_utils`**: `strdup`, `strndup`, `strtrim`,
  `strsplit`, `strjoin` (as strings alocadas).
- **`009_snake`**: `game_init` (o `body` da cobra).

**Onde NÃO usamos**:

- **`001_wc`**: não precisa (só lê e conta).
- **`002_cat`**: não precisa (só lê e escreve).
- **`005_getopt`**: não precisa (parsing de flags).
- **`006_hexdump`**: não precisa (buffer fixo de 16 bytes).
- **`008_rle`**: não precisa (buffer fixo de bytes).

**A regra**: use `malloc` **só quando precisar**. Se o tamanho é
fixo, ou pequeno, ou local, **não use**.

## Leitura recomendada

- *The C Programming Language*, Kernighan & Ritchie — seção 8.7
  (storage allocator).
- *Modern C*, Jens Gustedt — capítulo sobre alocação dinâmica.
- *Expert C Programming: Deep C Secrets*, Peter van der Linden —
  capítulo 7 (memory).

---

arataca89@gmail.com

Aulas de programação
