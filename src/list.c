#include "list.h"

/* Types **********************************************************************/

reftype(List,
        struct {
          void        *value;
          struct List *next;
        });
deftype(List);

/* API ************************************************************************/

List *list_new(Scope *scope, void *value) {
  List *list  = (List*)scope_alloc(scope, sizeof(List));
  list->type  = type(List);
  list->value = value;
  list->next  = NULL;
  return list;
}

List *list_cons(List *tail, void *value) {
  Scope *scope = scopeof(tail);
  List  *head  = scope_alloc(scope, sizeof(List));
  head->type   = type(List);
  head->value  = value;
  head->next   = tail;
  return head;
}

void list_foreach(List *head, void(*fun)(void *)) {
  List *current = head;
  while(current != NULL) {
    fun(current->value);
    current = current->next;
  }
}

void *list_foldl(List *head, void *acc0, void *(*fun)(void *acc, void *value)) {
  void *acc     = acc0;
  List *current = head;
  while(current != NULL) {
    acc = fun(acc, current->value);
    current = current->next;
  }
  return acc;
}
