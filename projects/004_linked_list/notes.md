# Notas de implementação — 004_list

Registro do raciocínio por trás do código. Não é tutorial: é o
"porquê" das decisões, incluindo os tropeços.

## Por que `struct List { Node *head; }` e não só `Node *head`?

Duas opções:

**Opção A — só `Node *head`**: a lista é acessada diretamente pela
cabeça. Simples, mas limitado.

**Opção B — `struct List { Node *head; }`**: um "container" que
envolve a cabeça.

Escolhemos a **Opção B** porque:

1. **É didático**: ensina que "lista" é um **tipo**, não só um
   ponteiro. O aluno vê `List *list` em todo lugar, e entende que
   a lista é uma entidade.
2. **Permite evoluir**: se quisermos adicionar `tail` (para
   `append` O(1)) ou `size` (para `size` O(1)), basta adicionar
   campos à `struct List`, sem mudar as funções.
3. **É o padrão em bibliotecas reais**: `std::list` (C++),
   `LinkedList` (Java), `Vec` (Rust) — todos têm um container.

## Por que só `int` e não `void *`?

Lista genérica (`void *`) exige casts (`*(int *)p`), e complica o
código para o aluno iniciante. O foco do `004` é **ponteiros e
estrutura**, não genericidade. Genérica fica para um projeto
futuro.

## O erro clássico de `free`: salvar o `next` antes

```c
/* ERRADO: use-after-free */
while (cur != NULL) {
    free(cur);
    cur = cur->next;   /* cur já foi liberado! */
}
```

**O problema**: `free(cur)` libera a memória, mas o ponteiro
`cur->next` ainda está lá... até que a memória seja reutilizada.
Ler `cur->next` depois de `free(cur)` é **use-after-free**
(comportamento indefinido).

**A correção**:

```c
while (cur != NULL) {
    Node *next = cur->next;   /* salva ANTES de free */
    free(cur);
    cur = next;
}
```

**Isso é obrigatório** em toda liberação de lista ligada. O
`list_free` usa esse padrão.

## O padrão `struct Node **pp` para remover

Remover um nó de uma lista ligada simples tem um caso especial
chato: se o nó a remover é a **cabeça**, o `head` precisa ser
atualizado. Se for um nó do **meio**, o `next` do anterior precisa
ser atualizado. Duas lógicas diferentes.

**Solução elegante**: usar `struct Node **pp` (ponteiro para
ponteiro):

```c
int list_remove(List *list, int value)
{
    Node **pp = &list->head;

    while (*pp != NULL) {
        if ((*pp)->value == value) {
            Node *victim = *pp;
            *pp = victim->next;   /* "pula" o nó removido */
            free(victim);
            return 1;
        }
        pp = &(*pp)->next;
    }
    return 0;
}
```

**Como funciona**:

- `pp` aponta para o **ponteiro** que aponta para o nó atual.
  - No início, `pp = &list->head` (o ponteiro `head`).
  - Se o nó atual é `head`, `*pp` é `head`, e `pp` é `&head`.
  - Se o nó atual é o segundo, `pp` é `&head->next`.
- `*pp = victim->next` — o ponteiro que apontava para o nó
  removido agora aponta para o **próximo**. É isso que "tira" o
  nó da lista.
- **Sem caso especial**: se o nó a remover é a cabeça, `pp` é
  `&list->head`, e `*pp = victim->next` funciona igual.

**Isso é o padrão mais elegante** para listas ligadas simples.
A alternativa (com `prev` e `cur`) tem dois casos: "remover a
cabeça" e "remover do meio".

## `push` O(1) vs. `append` O(n)

`push` insere no **início**:

```c
n->next = list->head;
list->head = n;
```

**O(1)** — não percorre a lista.

`append` insere no **fim**:

```c
Node *cur = list->head;
while (cur->next != NULL)
    cur = cur->next;
cur->next = n;
```

**O(n)** — percorre até o último nó.

**Isso é o trade-off** da lista ligada simples. Um array é o
contrário: inserir no fim é O(1) (se souber o tamanho), inserir no
início é O(n) (move todos).

**Se quiséssemos `append` O(1)**, precisaríamos de um ponteiro
para o `tail` na `struct List`:

```c
typedef struct {
    Node *head;
    Node *tail;   /* adicionar */
} List;
```

Aí `append` seria:

```c
if (list->tail) {
    list->tail->next = n;
    list->tail = n;
} else {
    list->head = list->tail = n;
}
```

**O(1)**. Mas o `remove` fica mais complexo (precisa atualizar o
`tail` se o nó removido for o último). Fica para um projeto
futuro. O `notes.md` registra a decisão: **simplicidade primeiro,
otimização depois**.

## `size` O(n) vs. O(1)

`list_size` percorre a lista contando:

```c
size_t n = 0;
for (Node *cur = list->head; cur != NULL; cur = cur->next)
    n++;
return n;
```

**O(n)**. Se quiséssemos O(1), adicionaríamos um campo `size` na
`struct List`, atualizado em `push`/`append`/`remove`:

```c
typedef struct {
    Node *head;
    size_t size;   /* adicionar */
} List;
```

Aí `size` seria O(1) (só retornar `list->size`). Mas toda operação
que muda a lista precisa atualizar `size` (mais código, mais
chance de bug). **Simplicidade primeiro**: O(n) é suficiente para
o escopo do `004`.

## `parse_int` com `strtol`: por que não `atoi`?

`atoi` **não detecta erros**:

```c
atoi("42abc")   /* retorna 42 (ignora "abc") */
atoi("abc")     /* retorna 0 (indistinguível de "0") */
atoi("")        /* retorna 0 */
```

**Em todos os casos inválidos, `atoi` retorna um valor**, e você
não sabe se a conversão foi válida. **`atoi` não serve** para
parsing robusto.

`strtol` **dá o `end`**:

```c
char *end;
long v = strtol(s, &end, 10);
```

`end` aponta para o **primeiro caractere não consumido** pela
conversão. Aí você verifica:

- **`*end != '\0'`**: sobrou lixo depois do número (`"42abc"`).
- **`end == s`**: nada foi convertido (`"abc"`, `""`).

**As duas condições juntas** cobrem todos os casos inválidos.

### O `while` para pular espaços

```c
while (*end == ' ' || *end == '\t')
    end++;
```

`strtol` **não consome** espaços depois do número. Se a string é
`"42 "` (com espaço no final), o `end` aponta para o espaço, e
`*end != '\0'` seria verdadeiro (rejeitaria `"42 "`, mesmo sendo
válido). O `while` pula o espaço, e o `end` aponta para o `'\0'`.
Aí `"42 "` é aceito.

**Sem o `while`**, `"42 "` seria rejeitado. **Com o `while`**, é
aceito. Mais amigável.

### O `errno` (melhoria futura)

`strtol` seta `errno = ERANGE` se o valor não cabe em `long`
(overflow). E o `int` pode não caber o `long` (em plataformas onde
`int` é 32 bits e `long` é 64 bits).

**Para o escopo do `004`**, os valores são pequenos (o usuário
digita `push 42`), então o `errno` é **overkill**. Mas para
parsing robusto de verdade (aceitar entrada arbitrária), seria:

```c
#include <errno.h>
#include <limits.h>

static int parse_int(const char *s, int *out)
{
    char *end;
    errno = 0;
    long v = strtol(s, &end, 10);

    while (*end == ' ' || *end == '\t')
        end++;

    if (*end != '\0' || end == s)
        return -1;

    if (errno == ERANGE || v < INT_MIN || v > INT_MAX)
        return -1;   /* overflow */

    *out = (int)v;
    return 0;
}
```

**Decisão**: manter a versão simples (sem `errno`) no `004`, e
registrar a versão com `errno` como melhoria futura.

## `fgets` com buffer fixo vs. `read_line` do `grep`

O `003_grep` usa `read_line` com `malloc`/`realloc` (linhas de
tamanho variável). O `004_linked_list` usa `fgets` com buffer fixo
de 256 bytes. **Por quê?**

**Escopo diferente**:

- **`grep`**: lê **linhas de arquivo**, que podem ter qualquer
  tamanho (10 KB, 1 MB). Precisa de buffer dinâmico.
- **`list`**: lê **comandos**, que são curtos (`push 42`,
  `print`). 256 bytes é **muito** mais que suficiente.

**Cada projeto escolhe a ferramenta certa para o escopo**. O
`notes.md` do `grep` registra a decisão de `malloc`/`realloc`; o
do `list` registra a decisão de `fgets`.

**Se um comando for maior que 255 caracteres**, o `fgets` trunca,
e o resto da linha fica no buffer para a próxima leitura. Isso
causaria um erro de parsing no próximo comando. Mas, para o
escopo do `004`, isso é **aceitável** (comandos são curtos). Se
fosse um problema, usaríamos `read_line`.

## `isatty` e `fileno`: o que são?

### `isatty`

`isatty(fd)` (POSIX) retorna `1` se o descritor de arquivo `fd` é
um **terminal**, `0` caso contrário.

- **Terminal** (interativo): `isatty` = 1.
- **Arquivo redirecionado** (`list < comandos.txt`): `isatty` = 0.
- **Pipe** (`echo "x" | list`): `isatty` = 0.

**No Windows**, a função é `_isatty` (com underline), em `<io.h>`.
O `#ifdef _WIN32` resolve:

```c
#ifdef _WIN32
#include <io.h>
#define isatty _isatty
#define fileno _fileno
#else
#include <unistd.h>
#endif
```

**Isso é portabilidade**: o mesmo código, com um `#ifdef` pequeno
e isolado. O `docs/style-guide.md` permite.

### `fileno`

`fileno(FILE *stream)` retorna o **descritor de arquivo** do
stream (um `int`). No Unix, `stdin` é o descritor `0`, `stdout` é
`1`, `stderr` é `2`.

**Por que `fileno`**: `isatty` espera um **descritor** (`int`),
não um `FILE *`. O `fileno(stdin)` converte.

**No Windows**, é `_fileno` (com underline). O `#define` resolve.

### Por que o prompt só aparece se `isatty`

**Interativo**: o usuário precisa saber que o programa está
esperando. O prompt `> ` sinaliza "digite algo".

**Batch**: a entrada vem de um arquivo ou pipe. O prompt **não**
faz sentido (o usuário não está digitando), e **polui** a saída.
Se o prompt aparecesse no batch, os gabaritos dos testes
quebrariam (a saída teria `> ` misturado com os resultados).

**A detecção com `isatty`** resolve os dois casos com o mesmo
código.

### `fflush(stdout)` após o prompt

```c
printf("> ");
fflush(stdout);
```

**Por que `fflush`**: `stdout` é **bufferizado**. Em um terminal,
é **line-buffered** (descarrega quando vê `\n`). Em um pipe, é
**full-buffered** (descarrega quando enche).

O prompt `> ` **não tem `\n`**. Sem `fflush`, ele fica no buffer,
e **não aparece** antes do `fgets`. O usuário vê o cursor piscando
**sem prompt**, digita o comando, e o prompt aparece **depois**
(junto com a saída).

**`fflush(stdout)` força a saída imediata**, e o prompt aparece
antes.

**Isso é uma sutileza de I/O** que vale registrar: prompts sem
`\n` precisam de `fflush`.

## A CLI: modo interativo vs. batch

O `list` lê comandos de **stdin**. Pode ser:

- **Interativo**: o usuário digita no terminal. O prompt `> `
  aparece.
- **Batch**: a entrada vem de um arquivo (`list < comandos.txt`)
  ou pipe (`echo "x" | list`). O prompt **não** aparece.

**O mesmo código** atende os dois, com `isatty`. Isso é
**elegante**: o programa não precisa de flags (`-i` para
interativo, `-b` para batch); ele **detecta** o modo.

**Alternativa** (que não usamos): uma flag `-i` para forçar
interativo, ou `-q` (quiet) para forçar batch. Mas o `isatty` é
mais simples e mais comum (o `python`, o `sqlite3`, o `gdb` fazem
isso).

## Por que `fgets` e não `scanf`?

`scanf("%d", &v)` parece mais simples, mas tem problemas:

1. **`scanf` não lê a linha inteira**. Se o usuário digita
   `push 42 extra`, o `scanf("%d")` lê `42` e deixa `extra` no
   buffer. A próxima leitura pega `extra`, que não é um comando
   válido.
2. **`scanf` não detecta overflow** facilmente.
3. **`scanf` com `%s`** tem o mesmo problema de buffer fixo que
   `fgets`, mas pior (não tem limite de tamanho, e pode
   estourar o buffer).

**`fgets` + `strtol`** é mais robusto: lê a linha inteira, e
converte com validação.

## Exit code do `find`: a simplificação do escopo

O `grep` real retorna:

- `0`: achou.
- `1`: não achou.
- `2`: erro.

O `list` retorna:

- `0`: sucesso (mesmo que o `find` não tenha encontrado).
- `1`: erro (alocação, lista não criada).

**Por que a diferença**: o `list` é **interativo** (o usuário vê
"nao encontrado" na tela, e continua). O exit code é sobre o
**programa**, não sobre o **resultado da busca**. Se o `find`
não encontra, o programa **rodou com sucesso** (a busca é que não
achou).

**Isso é uma simplificação** do escopo. Se o `list` fosse usado
em scripts (como o `grep`), faria sentido retornar 1 quando o
`find` não encontra. Mas, para o escopo do `004` (interativo), o
exit code 0 é razoável. O `notes.md` registra a decisão.

## Lições gerais deste projeto

1. **`struct` que aponta para si mesma** é o coração da lista
   ligada. O `typedef` permite `Node`, mas o `struct Node *next`
   dentro da struct precisa do `struct` (o `typedef` ainda não
   terminou).
2. **`malloc`/`free` estruturados**: alocar um nó, liberar um nó,
   liberar a lista inteira. O erro clássico é `free` sem salvar o
   `next`.
3. **`struct Node **pp`** evita o caso especial "remover a
   cabeça". É o padrão mais elegante.
4. **`push` O(1) vs. `append` O(n)**: o trade-off da lista ligada
   simples. Otimização (com `tail`) fica para depois.
5. **`strtol` em vez de `atoi`**: `atoi` não detecta erros.
   `strtol` dá o `end`, que permite validação.
6. **`isatty` + `fileno`**: detecta terminal vs. arquivo/pipe.
   Permite prompt só no interativo, e saída limpa no batch.
7. **`fflush(stdout)` após prompt**: sem `\n`, o prompt não
   aparece sem flush.
8. **`fgets` vs. `read_line`**: cada projeto escolhe a ferramenta
   certa para o escopo. Comandos curtos → `fgets`. Linhas longas
   → `malloc`/`realloc`.
9. **`const` como contrato**: `find`, `print`, `size` não
   modificam a lista.
10. **Simplicidade primeiro, otimização depois**: `append` O(n) e
    `size` O(n) são suficientes para o escopo. `tail` e `size`
    ficam para depois.
