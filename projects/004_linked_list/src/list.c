#include "list.h"

#include <stdio.h>
#include <stdlib.h>

List *list_create(void)
{
    List *l = malloc(sizeof *l);
    if (!l)
        return NULL;
    l->head = NULL;
    return l;
}

void list_free(List *list)
{
    if (!list)
        return;

    Node *cur = list->head;
    while (cur != NULL) {
        Node *next = cur->next;   /* salva ANTES de free */
        free(cur);
        cur = next;
    }

    free(list);
}

int list_push(List *list, int value)
{
    Node *n = malloc(sizeof *n);
    if (!n)
        return -1;

    n->value = value;
    n->next = list->head;
    list->head = n;
    return 0;
}

int list_append(List *list, int value)
{
    Node *n = malloc(sizeof *n);
    if (!n)
        return -1;

    n->value = value;
    n->next = NULL;

    if (list->head == NULL) {
        list->head = n;
        return 0;
    }

    Node *cur = list->head;
    while (cur->next != NULL)
        cur = cur->next;
    cur->next = n;
    return 0;
}

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

Node *list_find(const List *list, int value)
{
    for (Node *cur = list->head; cur != NULL; cur = cur->next) {
        if (cur->value == value)
            return cur;
    }
    return NULL;
}

void list_print(const List *list)
{
    for (Node *cur = list->head; cur != NULL; cur = cur->next)
        printf("%d\n", cur->value);
}

size_t list_size(const List *list)
{
    size_t n = 0;
    for (Node *cur = list->head; cur != NULL; cur = cur->next)
        n++;
    return n;
}