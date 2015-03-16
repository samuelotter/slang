#include "map.h"

#include "crc32.h"

enum NODE_TYPE {
  LEAF,
  BRANCH
};

typedef struct Leaf {
  char       type;
  uint32_t   hash;
  const void *key;
  const void *value;
} Leaf;

typedef struct Branch {
  char type;
  int  mask;
  void *elements[];
} Branch;

typedef union Node {
  char   type;
  Leaf   leaf;
  Branch branch;
} Node;

reftype(Map,
        struct {
          Node *node;
        });
deftype(Map);

/* Forward decls. *************************************************************/

static Node* insert(int level, Node *node, uint32_t hash, const void *key,
                    const void *value);
static Leaf *make_leaf(Scope *scope, uint32_t hash, const void *key,
                       const void *value);
static const void *lookup(int level, Node *node, uint32_t hash,
                          const void *key);

/* API ************************************************************************/

Map *map(Scope *scope) {
  Map *map     = scope_alloc(scope, sizeof(Map));
  map->node    = NULL;
  map->type    = type(Map);
  return map;
}

Map *map_insert(Map *map, const void *key, size_t key_size, const void *value) {
  Scope *scope  = scopeof(map);
  Node *node    = map->node;
  uint32_t hash = crc32(0, key, key_size);
  if (node == NULL) {
    node = (Node*)make_leaf(scope, hash, key, value);
  } else {
    node = insert(1, node, hash, key, value);
  }
  Map *new_map = scope_alloc(scope, sizeof(Map));
  new_map->node = node;
  new_map->type = type(Map);
  return (Map*)new_map;
}

const void *map_lookup(Map *map, const void *key, size_t key_size) {
  Node *node = map->node;
  if (node == NULL) {
    return NULL;
  }
  uint32_t hash = crc32(0, key, key_size);
  return lookup(1, node, hash, key);
}

static inline int bitindex(int mask, int bit) {
  return __builtin_popcount(mask & (bit - 1));
}

static inline int bitpos(int level, uint32_t hash) {
  return 1 << ((hash >> (32 - level * 4)) & 0xF);
}

Leaf *make_leaf(Scope *scope, uint32_t hash, const void *key,
                const void *value) {
  Leaf* node  = scope_alloc(scope, sizeof(Leaf));
  node->type  = LEAF;
  node->hash  = hash;
  node->key   = key;
  node->value = value;
  return node;
}

Node* insert(int level, Node *node, uint32_t hash, const void *key,
             const void *value) {
  Scope* scope = scopeof(node);
  int bit      = bitpos(level, hash);
  if (node->type == LEAF) {
    int leaf_bit   = bitpos(level, node->leaf.hash);
    int length     = 2;
    size_t size    = sizeof(Branch) + sizeof(Node*) * length;
    Branch *branch = scope_alloc(scope, size);
    branch->type   = BRANCH;
    branch->mask   = bit | leaf_bit;
    branch->elements[bitindex(branch->mask, bit)]      =
      make_leaf(scope, hash, key, value);
    branch->elements[bitindex(branch->mask, leaf_bit)] =
      make_leaf(scope, node->leaf.hash, node->leaf.key, node->leaf.value);
    return (Node*)branch;
  } else {
    int count = __builtin_popcount(node->branch.mask);
    if ((node->branch.mask & bit) == 0) {
      int length     = count > 8 ? 16 : count + 1;
      size_t size    = sizeof(Branch) + sizeof(Node*) * length;
      Branch *branch = scope_alloc(scope, size);
      branch->type   = BRANCH;
      branch->mask   = node->branch.mask | bit;
      int index      = bitindex(branch->mask, bit);
      memcpy(branch->elements, node->branch.elements, sizeof(Node*) * index - 1);
      memcpy(branch->elements + index, node->branch.elements + index, count - index);
      branch->elements[index] = make_leaf(scope, hash, key, value);
      return (Node*)branch;
    } else {
      return insert(level + 1, node, hash, key, value);
    }
  }
  return node;
}

const void *lookup(int level, Node *node, uint32_t hash, const void *key) {
  if (node->type == LEAF) {
    // TODO: Compare value
    return (node->leaf.key == key) ? node->leaf.value : NULL;
  } else {
    int bit = bitpos(level, hash);
    if ((node->branch.mask & bit) != 0) {
      int index = bitindex(node->branch.mask, bit);
      return lookup(level + 1, node->branch.elements[index], hash, key);
    }
  }
  return NULL;
}
