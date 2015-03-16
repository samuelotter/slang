#ifndef _TUPLE_H_
#define _TUPLE_H_

#include "scope.h"
#include "ref.h"

#include <stddef.h>

// Types -----------------------------------------------------------------------

decltype(Tuple, struct Tuple);

// API -------------------------------------------------------------------------

Tuple *tuple_new(Scope *scope, uint32_t size);

Ref    tuple_get(Tuple *tuple, uint32_t n);
Tuple *tuple_set(Tuple *tuple, uint32_t n, Ref value);

uint32_t tuple_hash32(uint32_t hash, Tuple *tuple);

#endif
