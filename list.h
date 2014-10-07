/*
 * Implements a doubly-linked list
 * Found in the book "Learn C the Hard Way" by Zed Shaw
*/

#pragma once

struct Node {
  void *data;
  struct Node *next;
  struct Node *prev;
};

typedef struct {
  struct Node *first;
  struct Node *last;
  int length;
} List;

List *List_create();

void List_clear(List *list);

void List_destroy(List *list);

void List_push(List *list, void *data);

void *List_pop(List *list);

void *List_pop(List *list);

void List_queue(List *list, void *data);

void *List_dequeue(List *list);

void List_print(List *list);

#define LIST_FOREACH(L, S, M, V) struct Node *_node = NULL;\
  struct Node *V = NULL;\
  for(V = _node = L->S; _node != NULL; V = _node = _node->M)
