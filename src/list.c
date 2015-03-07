#include "list.h"

List* list_new(Scope* scope, void* value) {
  List* list  = (List*)scope_alloc(scope, sizeof(List));
  list->value = value;
  return list;
}

List* list_cons(List* tail, void* value) {
  Scope* scope = scopeof(tail);
  List*  head  = scope_alloc(scope, sizeof(List));
  head->value  = value;
  head->next   = tail;
  return head;
}
