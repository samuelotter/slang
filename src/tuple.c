#include "tuple.h"

#include <assert.h>

Tuple* tuple_new(Scope* scope, size_t n) {
  Tuple* tuple = (Tuple*)scope_alloc(scope, sizeof(Tuple) + sizeof(Ref) * n);
  tuple->size = n;
  return tuple;
}

Ref tuple_get(Tuple* tuple, size_t n) {
  assert(n < tuple->size);
  return tuple->elements[n];
}

Tuple* tuple_set(Tuple* tuple, size_t n, Ref value) {
  assert(n < tuple->size);
  tuple->elements[n] = value;
  return tuple;
}
