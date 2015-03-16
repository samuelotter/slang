#include "scope.h"
#include "list.h"
#include "tuple.h"
#include "atom.h"
#include "map.h"

#include <stdio.h>
#include <stdlib.h>

int* do_stuff(Scope* scope) {
  int* var = (int*)scope_alloc(scope, sizeof(int));
  *var     = 42;
  return var;
}

void *print_val(void *count, Ref value) {
  int* c = count;
  printf("%d: %p\n", *c, value.ptr);
  *c = *c + 1;
  return count;
}

#define assert(EXPR)                                                    \
  if (!(EXPR)) {                                                        \
    printf("assertion failed: '%s' at %s:%d\n",                         \
           #EXPR, __FILE__, __LINE__);                                  \
    exit(1);                                                            \
  }

#define assert_eq(EXPR1, EXPR2)                                         \
  if ((EXPR1) != (EXPR2)) {                                             \
    printf("assertion failed: '%s == %s'\n"                             \
           "expected: %p but got %p at %s:%d",                          \
           #EXPR1, #EXPR2, (EXPR1), (EXPR2), __FILE__, __LINE__);       \
    exit(1);                                                            \
  }

int main(int argc, char** argv) {
  Scope *scope = scope_begin();

  Atom *atom1 = atom("the_atom");
  Atom *atom2 = atom("the_atom");
  Atom *atom3 = atom("another_atom");

  assert(atom1 == atom2);
  assert(atom1 != atom3);

  List* a = list_new(scope, (Ref)atom1);
  List* b = list_cons(a, (Ref)atom2);
  List* c = list_cons(b, (Ref)atom3);

  int* var = do_stuff(scope);

  list_foldl(c, var, &print_val);

  Tuple* tuple = tuple_new(scope, 3);
  tuple_set(tuple, 0, (Ref)a);

  char *large_obj = scope_alloc(scope, 16384);
  memset(large_obj, 42, 16384);
  assert(large_obj != NULL);

  printf("var = %d, %s\n", *var, atom1->name);

  Map *map1 = map(scope);
  map1      = map_insert(map1, (Ref)atom1, (Ref)tuple);
  map1      = map_insert(map1, (Ref)atom3, (Ref)c);

  assert_eq(tuple, map_lookup(map1, (Ref)atom1).tuple);
  assert_eq(c, map_lookup(map1, (Ref)atom3).list);

  scope_destroy(scope);
  return 0;
}
