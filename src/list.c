#include "list.h"

#include "ref.h"

/* Types **********************************************************************/

deftype(List);

typedef enum {
  EMPTY,
  VALUE
} CELL_TYPE;

/* API ************************************************************************/

List *list_new(Scope *scope) {
  List *list   = (List*)scope_alloc(scope, sizeof(List));
  list->header = ref_header(TYPEID_LIST, EMPTY);
  list->value  = (Ref)NULL;
  list->next   = NULL;
  return list;
}

int list_is_empty(List *list) {
  return ref_data(list) == EMPTY;
}

List *list_cons(List *tail, Ref value) {
  Scope *scope = scopeof(tail);
  List  *head  = scope_alloc(scope, sizeof(List));
  head->header = ref_header(TYPEID_LIST, VALUE);
  head->value  = value;
  head->next   = tail;
  return head;
}

void list_foreach(List *head, void(*fun)(Ref)) {
  List *current = head;
  while(!list_is_empty(current)) {
    fun(current->value);
    current = current->next;
  }
}

void *list_foldl(List *head, void *acc0, void *(*fun)(void *acc, Ref value)) {
  void *acc     = acc0;
  List *current = head;
  while(!list_is_empty(current)) {
    acc = fun(acc, current->value);
    current = current->next;
  }
  return acc;
}

List *list_reverse(List *list) {
  Scope *scope = scopeof(list);
  List  *head  = list_new(scope);
  while (!list_is_empty(list)) {
    head = list_cons(head, list->value);
    list = list->next;
  }
  return head;
}

uint32_t list_hash32(uint32_t hash, List *list) {
  List *current = list;
  while (!list_is_empty(current)) {
    hash = ref_hash32(hash, current->value);
    current = list->next;
  }
  return hash;
}
