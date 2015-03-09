#include "list.h"

List* list_new(Scope* scope, Ref value) {
  List* list  = (List*)scope_alloc(scope, sizeof(List));
  list->value = value;
  list->next  = NULL;
  return list;
}

List* list_cons(List* tail, Ref value) {
  Scope* scope = scopeof(tail);
  List*  head  = scope_alloc(scope, sizeof(List));
  head->value  = value;
  head->next   = tail;
  return head;
}

void list_foreach(List* head, void(*fun)(Ref)) {
  List* current = head;
  while(current != NULL) {
    fun(current->value);
    current = current->next;
  }
}

Ref list_foldl(List* head, Ref acc0, Ref(*fun)(Ref acc, Ref value)) {
  Ref   acc     = acc0;
  List* current = head;
  while(current != NULL) {
    acc = fun(acc, current->value);
    current = current->next;
  }
  return acc;
}
