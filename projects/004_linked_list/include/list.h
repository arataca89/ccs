#ifndef LIST_H
#define LIST_H

#include <stddef.h>

/* Nó da lista ligada simples. */
typedef struct Node {
    int value;
    struct Node *next;
} Node;

/* Container da lista. Envolve a cabeça para permitir evoluir
 * (adicionar tail, size) sem mudar as funções. */
typedef struct {
    Node *head;
} List;

/* Cria uma lista vazia (head = NULL). */
List *list_create(void);

/* Libera a lista inteira e o container. */
void list_free(List *list);

/* Insere no início. O(1). Retorna 0 em sucesso, -1 em erro. */
int list_push(List *list, int value);

/* Insere no fim. O(n). Retorna 0 em sucesso, -1 em erro. */
int list_append(List *list, int value);

/* Remove o primeiro nó com `value`. Retorna 1 se removeu,
 * 0 se não encontrou. */
int list_remove(List *list, int value);

/* Busca o primeiro nó com `value`. Retorna o nó, ou NULL. */
Node *list_find(const List *list, int value);

/* Imprime os valores, um por linha. */
void list_print(const List *list);

/* Retorna o número de nós. O(n). */
size_t list_size(const List *list);

#endif /* LIST_H */