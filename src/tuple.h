#ifndef _TUPLE_H_
#define _TUPLE_H_

#include "scope.h"

#include <stddef.h>

// Types -----------------------------------------------------------------------

decltype(Tuple, struct Tuple);

// API -------------------------------------------------------------------------

Tuple *tuple_new(Scope *scope, size_t size);

void  *tuple_get(Tuple *tuple, size_t n);
Tuple *tuple_set(Tuple *tuple, size_t n, void *value);

#endif
