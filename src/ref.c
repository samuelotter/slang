#include "ref.h"

#include "atom.h"
#include "list.h"
#include "tuple.h"
#include "map.h"

#include <assert.h>

uint32_t ref_hash32(uint32_t hash, Ref ref) {
  switch (ref_type(ref.header)) {
  case TYPEID_ATOM:
    return atom_hash32(hash, ref.atom);
  case TYPEID_LIST:
    return list_hash32(hash, ref.list);
  case TYPEID_TUPLE:
    return tuple_hash32(hash, ref.tuple);
  case TYPEID_MAP:
    return map_hash32(hash, ref.map);
  default:
    assert(0);
  }
}
