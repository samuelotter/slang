#ifndef _LIST_H_
#define _LIST_H_

#include "scope.h"

#include "type.h"

decltype(List, struct List);

List* list_new(Scope* scope, void *value);
List* list_cons(List* list, void *value);

void  list_foreach(List* list, void(*fun)(void *));
void *list_foldl(List* list, void * acc, void *(*fun)(void *, void *));

#endif
