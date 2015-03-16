#include "test_util.h"

#include <map.h>
#include <atom.h>
#include <tuple.h>

test_suite(map_tests, {
    Scope *scope = scope_begin();
    test_case(create_map, {
        Map *map1 = map(scope);
        assert(map1 != NULL);
      });

    test_case(insert, {
        Atom *key1 = atom("key1");
        Atom *key2 = atom("key2");
        Atom *value1 = atom("value1");
        Atom *value2 = atom("value2");
        Map *map1 = map(scope);
        map1      = map_insert(map1, (Ref)key1, (Ref)value1);
        map1      = map_insert(map1, (Ref)key2, (Ref)value2);
        assert_eq(value1, map_lookup(map1, (Ref)key1).atom);
        assert_eq(value2, map_lookup(map1, (Ref)key2).atom);
      });
    scope_destroy(scope);
  });
