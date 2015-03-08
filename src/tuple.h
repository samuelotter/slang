#ifndef _TUPLE_H_
#define _TUPLE_H_

#include "scope.h"

#include <stddef.h>

// Types -----------------------------------------------------------------------

typedef struct {
  size_t  size;
  Ref     elements[0];
} Tuple;

// API -------------------------------------------------------------------------

Tuple* tuple_new(Scope* scope, size_t size);

Ref    tuple_get(Tuple* tuple, size_t n);
Tuple* tuple_set(Tuple* tuple, size_t n, Ref value);

#endif
