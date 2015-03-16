#ifndef _LIST_H_
#define _LIST_H_

#include "scope.h"

#include "type.h"
#include "ref.h"

decltype(List, struct List);

List* list_new(Scope* scope, Ref value);
List* list_cons(List* list, Ref value);

void  list_foreach(List* list, void(*fun)(Ref));
void *list_foldl(List* list, void * acc, void *(*fun)(void *, Ref));

uint32_t list_hash32(uint32_t hash, List *list);

#endif
