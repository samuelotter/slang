#include "map.h"

#include "crc32.h"
#include "ref.h"

#include <assert.h>

enum NODE_TYPE {
  LEAF,
  BRANCH
};

reftype(Map,
        Ref elements[];
        );
deftype(Map);

#define node_data(TYPE, MASK)                    \
  ((TYPE & 0x1) | ((MASK & 0xFFFF) << 1))
#define node_type(NODE) (ref_data(NODE) & 0x1)
#define node_mask(NODE) ((ref_data(NODE) >> 1) & 0xFFFF)

/* Forward decls. *************************************************************/

static Map *insert(int level, Map *node, uint32_t hash, const Ref key,
                    const Ref value);
static Map *make_leaf(Scope *scope, int bit, const Ref key,
                       const Ref value);
static const Ref lookup(int level, Map *node, uint32_t hash,
                          const Ref key);

static inline int bitindex(int mask, int bit);
static inline int bitpos(int level, uint32_t hash);

/* API ************************************************************************/

Map *map(Scope *scope) {
  Map *map     = scope_alloc(scope, sizeof(Map));
  map->header  = ref_header(TYPEID_MAP, node_data(BRANCH, 0));
  return map;
}

Map *map_insert(Map *map, const Ref key, const Ref value) {
  Scope *scope  = scopeof(map);
  uint32_t hash = ref_hash32(0, key);
  if (node_mask(map) == 0x0) {
    return make_leaf(scope, bitpos(1, hash), key, value);
  } else {
    return insert(1, map, hash, key, value);
  }
}

const Ref map_lookup(Map *map, const Ref key) {
  assert(map != NULL);
  uint32_t hash = ref_hash32(0, key);
  return lookup(1, map, hash, key);
}

uint32_t map_hash32(uint32_t hash, Map *map) {
  if (node_type(map) == LEAF) {
    return ref_hash32(ref_hash32(hash, map->elements[0]), map->elements[1]);
  } else {
    int count = __builtin_popcount(node_mask(map));
    for (int i = 0; i < count; i++) {
      hash = ref_hash32(hash, map->elements[i]);
    }
    return hash;
  }
}

/* Internal Functions *********************************************************/

static inline int bitindex(int mask, int bit) {
  return __builtin_popcount(mask & (bit - 1));
}

static inline int bitpos(int level, uint32_t hash) {
  return 1 << ((hash >> (32 - level * 4)) & 0xF);
}

Map *make_leaf(Scope *scope, int bit, const Ref key,
                const Ref value) {
  Map *node         = scope_alloc(scope, sizeof(Map) + sizeof(Ref) * 2);
  node->header      = ref_header(TYPEID_MAP, node_data(LEAF, bit));
  node->elements[0] = key;
  node->elements[1] = value;
  return node;
}

Map* insert(int level, Map *node, uint32_t hash, const Ref key,
             const Ref value) {
  // TODO: Handle collisions
  // TODO: Handle overflow
  // TODO: Compare keys
  Scope* scope = scopeof(node);
  int bit      = bitpos(level, hash);
  if (node_type(node) == LEAF) {
    uint32_t leaf_hash = ref_hash32(0, node->elements[0]);
    int leaf_bit       = bitpos(level, leaf_hash);
    if (leaf_bit == bit) {
      assert(0 /* Collision */);
    }
    int length         = 2;
    size_t size        = sizeof(Map) + sizeof(Ref) * length;
    Map *branch        = scope_alloc(scope, size);
    int mask           = bit | leaf_bit;
    branch->header     = ref_header(TYPEID_MAP, node_data(BRANCH, mask));
    branch->elements[bitindex(mask, bit)] = (Ref)make_leaf(scope, bit, key, value);
    branch->elements[bitindex(mask, leaf_bit)] = (Ref)node;
    return branch;
  } else {
    int mask  = node_mask(node);
    int count = __builtin_popcount(mask);
    if ((mask & bit) == 0 && count < 16) {
      int length     = count > 16 ? 16 : count + 1;
      size_t size    = sizeof(Map) + sizeof(Ref) * length;
      Map *branch    = scope_alloc(scope, size);
      mask           = mask | bit;
      branch->header = ref_header(TYPEID_MAP, node_data(BRANCH, mask));
      int index      = bitindex(mask, bit);
      memcpy(branch->elements, node->elements, sizeof(Ref) * (index - 1));
      memcpy(branch->elements + index, node->elements + index, sizeof(Ref) * (count - index));
      branch->elements[index] = (Ref)make_leaf(scope, bit, key, value);
      return branch;
    } else {
      return insert(level + 1, node, hash, key, value);
    }
  }
  assert(0);
  return node;
}

const Ref lookup(int level, Map *node, uint32_t hash, const Ref key) {
  if (node_type(node) == LEAF) {
    // TODO: Compare key
    return (node->elements[0].ptr == key.ptr) ? node->elements[1] : (Ref)NULL;
  } else {
    int bit  = bitpos(level, hash);
    int mask = node_mask(node);
    if ((mask & bit) != 0) {
      int index = bitindex(mask, bit);
      return lookup(level + 1, node->elements[index].map, hash, key);
    }
  }
  return (Ref)NULL;
}
