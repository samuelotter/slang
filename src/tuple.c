#include "tuple.h"

#include <assert.h>

reftype(Tuple,
        struct {
        size_t size;
        void   *elements[0];
        });
deftype(Tuple);

Tuple *tuple_new(Scope *scope, size_t n) {
  Tuple *tuple = (Tuple*)scope_alloc(scope, sizeof(Tuple) + sizeof(void*) * n);
  tuple->size = n;
  return tuple;
}

void *tuple_get(Tuple *tuple, size_t n) {
  assert(n < tuple->size);
  return tuple->elements[n];
}

Tuple *tuple_set(Tuple* tuple, size_t n, void *value) {
  assert(n < tuple->size);
  tuple->elements[n] = value;
  return tuple;
}
