#ifndef _LIST_H_
#define _LIST_H_

#include "scope.h"

typedef struct List {
  void* value;
  struct List* next;
} List;

List* list_new(Scope* scope, void* value);
List* list_cons(List* list, void* value);

#endif
