#include "tuple.h"

#include <assert.h>
#include <ref.h>

deftype(Tuple);

Tuple *tuple_new(Scope *scope, uint32_t n) {
  Tuple *tuple  = (Tuple*)scope_alloc(scope, sizeof(Tuple) + sizeof(Ref) * n);
  tuple->header = ref_header(TYPEID_TUPLE, n);
  return tuple;
}

Ref tuple_get(Tuple *tuple, uint32_t n) {
  uint32_t size = ref_data(tuple);
  assert(n < size);
  return tuple->elements[n];
}

Tuple *tuple_set(Tuple* tuple, uint32_t n, Ref value) {
  uint32_t size = ref_data(tuple);
  assert(n < size);
  tuple->elements[n] = value;
  return tuple;
}

uint32_t tuple_hash32(uint32_t hash, Tuple *tuple) {
  int size = ref_data(tuple);
  for (size_t i = 0; i < size; i++) {
    hash = ref_hash32(hash, tuple->elements[i]);
  }
  return hash;
}
