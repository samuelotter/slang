#include "list.h"

#include "ref.h"

/* Types **********************************************************************/

reftype(List,
        struct {
          Ref          value;
          struct List *next;
        });
deftype(List);

/* API ************************************************************************/

List *list_new(Scope *scope, Ref value) {
  List *list   = (List*)scope_alloc(scope, sizeof(List));
  list->header = ref_header(TYPEID_LIST, 0);
  list->value  = value;
  list->next   = NULL;
  return list;
}

List *list_cons(List *tail, Ref value) {
  Scope *scope = scopeof(tail);
  List  *head  = scope_alloc(scope, sizeof(List));
  head->header = ref_header(TYPEID_LIST, 0);
  head->value  = value;
  head->next   = tail;
  return head;
}

void list_foreach(List *head, void(*fun)(Ref)) {
  List *current = head;
  while(current != NULL) {
    fun(current->value);
    current = current->next;
  }
}

void *list_foldl(List *head, void *acc0, void *(*fun)(void *acc, Ref value)) {
  void *acc     = acc0;
  List *current = head;
  while(current != NULL) {
    acc = fun(acc, current->value);
    current = current->next;
  }
  return acc;
}

uint32_t list_hash32(uint32_t hash, List *list) {
  List *current = list;
  while (current != NULL) {
    hash = ref_hash32(hash, current->value);
  }
  return hash;
}
