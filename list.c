#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "list.h"

List *List_create()
{
  List *list = malloc(sizeof(List));
  assert(list != NULL);

  list->length = 0;

  list->first = NULL;
  list->last = NULL;

  return list;
}

void List_clear(List *list) {
  if (list->length > 0) {
    struct Node *node = list->first;

    for (int i = 0; i < list->length; i++) {
      struct Node *next = node->next;
      free(node);
      node = next;
    }
  }

  list->first = NULL;
  list->last = NULL;
  list->length = 0;
}

void List_destroy(List *list)
{
  assert(list != NULL);

  List_clear(list);

  free(list);
}

void List_push(List *list, void *data) {
  struct Node *node = malloc(sizeof(struct Node));
  assert(node != NULL);

  node->data = data;
  node->next = NULL;

  // list is empty
  if (list->length == 0) {
    list->first = node;
    list->last = node;
  } else { // list is not empty
    // link nodes together
    list->last->next = node;
    node->prev = list->last;

    list->last = node;
  }

  list->length++;
}

void *List_pop(List *list) {
  if (list->length == 0) {
    return NULL;
  }

  void *data = list->last->data;

  if (list->length == 1) {
    free(list->last);
    list->last = NULL;
    list->first = NULL;
  }

  if (list->length > 1) {
    struct Node *removee = list->last;
    list->last = removee->prev;
    free(removee);
    list->last->next = NULL;
  }

  list->length--;

  return data;
}

void List_queue(List *list, void *data) {
  struct Node *node = malloc(sizeof(struct Node));
  assert(node != NULL);

  node->data = data;

  // list is empty
  if (list->length == 0) {
    list->first = node;
    list->last = node;
  } else { // list not empty
    // link nodes together
    list->first->prev = node;
    node->next = list->first;

    list->first = node;
  }

  list->length++;
}

void *List_dequeue(List *list) {
  if (list->length == 0) {
    return NULL;
  }

  void *data = list->first->data;

  if (list->length == 1) {
    free(list->first);
    list->first = NULL;
    list->last = NULL;
  }

  if (list->length > 1) {
    list->first = list->first->next;
    free(list->first->prev);
    list->first->prev = NULL;
  }

  list->length--;

  return data;
}

void List_clone(List *list);

void List_print(List *list)
{
  printf("List is at memory location: %p\n", list);
  printf("Length: %d\n", list->length);

  struct Node *node = list->first;

  for (int i = 0; i < list->length; i++) {
    // printf("\t%d\n", node->data);

    node = node->next;
  }
}
