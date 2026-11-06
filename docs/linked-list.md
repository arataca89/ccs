# Listas ligadas

> Um array é uma fila de cadeiras numeradas. Uma lista ligada é
> uma caça ao tesouro: cada pista aponta para a próxima.

## O problema: arrays têm tamanho fixo

Em C, um array tem **tamanho fixo**:

```c
int v[10];   /* 10 inteiros, sempre */
```

Se você precisa de 11, não cabe. Se precisa de 3, desperdiça 7. Você
pode alocar dinamicamente:

```c
int *v = malloc(n * sizeof(int));
```

Mas aí precisa saber `n` **antes** de usar. Se `n` muda (inserir,
remover), você tem que **realocar** (`realloc`) e **mover** os
elementos. Para inserir no meio, é pior: move todos os elementos
posteriores.

**Lista ligada** resolve isso: cresce e encolhe **um elemento por
vez**, sem mover os outros, e sem saber o tamanho total de antemão.

## O conceito: cada nó aponta para o próximo

Uma lista ligada é uma sequência de **nós**. Cada nó tem:

- Um **valor** (o dado).
- Um **ponteiro** para o próximo nó (ou `NULL`, se for o último).

Em C:

```c
struct Node {
    int value;
    struct Node *next;
};
```

Visualmente, uma lista com os valores `10 → 20 → 30`:

```
+------+------+    +------+------+    +------+------+
|  10  | next-+--->|  20  | next-+--->|  30  | NULL |
+------+------+    +------+------+    +------+------+
```

O primeiro nó é chamado de **cabeça** (`head`). A lista inteira
é acessada a partir dela:

```c
struct Node *head = ...;
```

Se `head == NULL`, a lista está **vazia**.

### Analogia: caça ao tesouro

Um array é uma **fila de cadeiras numeradas**: você sabe que a
cadeira 0 é a primeira, a 1 é a segunda, etc. Para chegar à 5,
vai direto.

Uma lista ligada é uma **caça ao tesouro**: a primeira pista está
num lugar conhecido (a `head`), e cada pista diz onde está a
próxima. Para chegar à 5, você **segue as pistas** uma a uma. Não
dá para "pular" para a 5 sem passar pelas anteriores.

**Isso é o trade-off**:

- **Array**: acesso direto (`v[5]`), mas tamanho fixo (ou realocação
  cara).
- **Lista ligada**: cresce/encolhe fácil, mas acesso **sequencial**
  (para chegar ao 5º, percorre os 4 anteriores).

## Como implementar em C

### 1. O nó

```c
#include <stdlib.h>

struct Node {
    int value;
    struct Node *next;
};
```

`struct Node` tem um ponteiro para **si mesma** (`struct Node
*next`). Isso é permitido em C: a struct pode conter um ponteiro
para o próprio tipo, porque o **tamanho** do ponteiro é conhecido
(mesmo que o tamanho da struct não esteja completo ainda).

**Isso é o coração da lista ligada**: a auto-referência.

### 2. Criar um nó

```c
struct Node *node_create(int value)
{
    struct Node *n = malloc(sizeof *n);
    if (!n)
        return NULL;
    n->value = value;
    n->next = NULL;
    return n;
}
```

**Pontos importantes**:

- `malloc(sizeof *n)` — `sizeof *n` é o tamanho da struct, sem
  precisar repetir o tipo. Se o tipo mudar, o `sizeof` acompanha.
- `if (!n) return NULL` — **sempre** verifique se `malloc`
  retornou `NULL` (memória esgotada).
- `n->next = NULL` — o novo nó **não** aponta para nada ainda.

### 3. Inserir no início (push)

```c
void list_push(struct Node **head, int value)
{
    struct Node *n = node_create(value);
    if (!n)
        return;   /* erro: memória esgotada */
    n->next = *head;
    *head = n;
}
```

**Pontos importantes**:

- **`struct Node **head`** (ponteiro para ponteiro) — porque a
  função **modifica** a `head` (o novo nó vira a cabeça). Se
  fosse `struct Node *head`, a modificação seria local e se
  perderia ao retornar.
- **`n->next = *head`** — o novo nó aponta para a cabeça antiga.
- **`*head = n`** — a cabeça agora é o novo nó.

Visualmente:

```
Antes:  head -> [10] -> [20] -> NULL
Depois: head -> [5] -> [10] -> [20] -> NULL
```

Inserir no início é **O(1)**: não percorre a lista.

### 4. Inserir no fim (append)

```c
void list_append(struct Node **head, int value)
{
    struct Node *n = node_create(value);
    if (!n)
        return;

    if (*head == NULL) {
        *head = n;
        return;
    }

    struct Node *cur = *head;
    while (cur->next != NULL)
        cur = cur->next;
    cur->next = n;
}
```

**Ponto importante**: inserir no fim é **O(n)** — percorre a
lista até o último nó. (Uma lista duplamente ligada, ou manter um
ponteiro para o `tail`, resolveria isso. Fica para depois.)

### 5. Buscar (find)

```c
struct Node *list_find(struct Node *head, int value)
{
    for (struct Node *cur = head; cur != NULL; cur = cur->next) {
        if (cur->value == value)
            return cur;
    }
    return NULL;
}
```

**Ponto importante**: o `for` percorre a lista com `cur = cur->next`.
Quando `cur == NULL`, chegou ao fim. **Nunca** dereferencie `cur`
antes de verificar se é `NULL`.

### 6. Remover (remove)

```c
int list_remove(struct Node **head, int value)
{
    struct Node **pp = head;   /* ponteiro para o ponteiro do nó */

    while (*pp != NULL) {
        if ((*pp)->value == value) {
            struct Node *victim = *pp;
            *pp = victim->next;   /* "pula" o nó removido */
            free(victim);
            return 1;             /* removeu */
        }
        pp = &(*pp)->next;
    }
    return 0;   /* não encontrou */
}
```

**Pontos importantes**:

- **`struct Node **pp`** — ponteiro para o **ponteiro** que aponta
  para o nó atual. Isso permite remover o nó **sem** caso especial
  para a cabeça.
- **`*pp = victim->next`** — o ponteiro que apontava para o nó
  removido agora aponta para o **próximo**. É isso que "tira" o
  nó da lista.
- **`free(victim)`** — libera a memória. **Só depois** de salvar o
  `next` (senão, perde o resto da lista).
- **`pp = &(*pp)->next`** — avança para o próximo nó.

**Esse é o ponto mais sutil da lista ligada.** O padrão
`struct Node **pp` evita o caso especial "remover a cabeça", que
seria necessário se usássemos `struct Node *prev` + `struct Node
*cur`.

### 7. Imprimir (print)

```c
void list_print(struct Node *head)
{
    for (struct Node *cur = head; cur != NULL; cur = cur->next)
        printf("%d\n", cur->value);
}
```

### 8. Liberar (free)

```c
void list_free(struct Node *head)
{
    while (head != NULL) {
        struct Node *next = head->next;
        free(head);
        head = next;
    }
}
```

**Ponto importante**: **salve o `next` antes de `free`**. Se você
fizer `free(head); head = head->next;`, o `head->next` é
**use-after-free** (comportamento indefinido). O padrão
`struct Node *next = head->next; free(head); head = next;` é
**obrigatório**.

**Versão recursiva** (mais elegante, mas usa pilha):

```c
void list_free_rec(struct Node *head)
{
    if (head == NULL)
        return;
    list_free_rec(head->next);
    free(head);
}
```

## Os erros mais comuns

### 1. `free` sem salvar o `next`

```c
/* ERRADO: use-after-free */
while (head != NULL) {
    free(head);
    head = head->next;   /* head já foi liberado! */
}
```

**Sempre** salve o `next` antes de `free`.

### 2. Perder a cabeça

```c
/* ERRADO: perdeu a referência à lista */
struct Node *head = ...;
head = head->next;   /* agora o primeiro nó está perdido (memory leak) */
```

Se você quer "avançar" sem perder a lista, use uma variável
auxiliar (`struct Node *cur = head`), não modifique a `head`.

### 3. Não verificar `malloc`

```c
/* ERRADO: se malloc falhar, n é NULL, e n->value é segfault */
struct Node *n = malloc(sizeof *n);
n->value = 42;
```

**Sempre** verifique `if (!n) return ...` após `malloc`.

### 4. Modificar a `head` sem ponteiro para ponteiro

```c
/* ERRADO: a modificação é local, e se perde ao retornar */
void list_push_wrong(struct Node *head, int value)
{
    struct Node *n = node_create(value);
    n->next = head;
    head = n;   /* modifica a cópia local, não a original */
}
```

**Use `struct Node **head`** para modificar a cabeça.

### 5. Esquecer de liberar

Toda lista alocada com `malloc` precisa ser liberada com `free`.
Se o programa termina sem liberar, o sistema operacional recupera
a memória — mas em programas de longa duração (servidores,
bibliotecas), **memory leak** é um bug sério.

## Lista ligada vs. array

| Aspecto | Array | Lista ligada |
|---|---|---|
| Tamanho | Fixo (ou realocação cara) | Dinâmico (cresce/encolhe) |
| Acesso | Direto (`v[i]`, O(1)) | Sequencial (O(n)) |
| Inserir no início | O(n) (move todos) | O(1) |
| Inserir no fim | O(1) (se souber o tamanho) | O(n) (percorre até o fim) |
| Inserir no meio | O(n) (move todos) | O(n) (percorre até o ponto) |
| Memória | Contígua (cache-friendly) | Espalhada (cache-unfriendly) |
| Complexidade | Simples | Ponteiros, `malloc`/`free` |

**Quando usar cada um**:

- **Array**: quando o tamanho é conhecido, ou quando o acesso
  direto é importante (ex.: `v[i]` em loop).
- **Lista ligada**: quando o tamanho é desconhecido, ou quando
  inserir/remover no início é frequente (ex.: pilha, fila).

## Onde listas ligadas aparecem

- **Pilhas e filas** (implementação clássica).
- **Tabelas hash** (encadeamento de colisões).
- **Grafos** (listas de adjacência).
- **Compiladores** (tabela de símbolos, árvore sintática).
- **Sistemas operacionais** (lista de processos prontos, lista
  de blocos livres).
- **Editores de texto** (buffer de linhas).
- **Histórico de navegação** (botão "voltar").
- **Blockchain** (cada bloco aponta para o anterior).

## Lista duplamente ligada (adiantamento)

Uma **lista duplamente ligada** tem, em cada nó, um ponteiro para
o **próximo** e um para o **anterior**:

```c
struct DNode {
    int value;
    struct DNode *next;
    struct DNode *prev;
};
```

Permite percorrer nos dois sentidos e remover um nó **sem**
percorrer a lista até ele (basta ter o ponteiro). Mas gasta mais
memória (dois ponteiros por nó) e tem mais casos de borda
(atualizar `prev` e `next` ao inserir/remover). Fica para um
projeto futuro.

## Leitura recomendada

- *The C Programming Language*, Kernighan & Ritchie — capítulo 6
  (structs) tem um exemplo de lista ligada (tabela de símbolos).
- *Algorithms in C*, Robert Sedgewick — capítulo sobre listas.
- *Introduction to Algorithms*, Cormen et al. — capítulo 10
  (estruturas de dados elementares).
