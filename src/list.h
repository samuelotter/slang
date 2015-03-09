#ifndef _LIST_H_
#define _LIST_H_

#include "scope.h"

typedef struct List {
  Ref          value;
  struct List* next;
} List;

List* list_new(Scope* scope, Ref value);
List* list_cons(List* list, Ref value);

void  list_foreach(List* list, void(*fun)(Ref));
Ref   list_foldl(List* list, Ref acc, Ref(*fun)(Ref, Ref));

#endif
