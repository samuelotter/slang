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

void *print_val(void *count, void *value) {
  int* c = count;
  printf("%d: %p\n", *c, value);
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

  List* a = list_new(scope, (void*)3);
  List* b = list_cons(a, (void*)2);
  List* c = list_cons(b, (void*)1);

  int* var = do_stuff(scope);

  list_foldl(c, var, &print_val);

  Tuple* tuple = tuple_new(scope, 3);
  tuple_set(tuple, 0, var);

  Atom *atom1 = atom("the_atom");
  Atom *atom2 = atom("the_atom");
  Atom *atom3 = atom("another_atom");

  assert(atom1 == atom2);
  assert(atom1 != atom3);

  char *large_obj = scope_alloc(scope, 16384);
  memset(large_obj, 42, 16384);
  assert(large_obj != NULL);

  printf("var = %d, %s\n", *var, atom1->name);

  Map *map1 = map(scope);
  map1      = map_insert(map1, atom1, sizeof(Atom), tuple);
  map1      = map_insert(map1, atom3, sizeof(Atom), large_obj);

  assert_eq(tuple, (Tuple*)map_lookup(map1, atom1, sizeof(Atom)));
  assert_eq(large_obj, (char*)map_lookup(map1, atom3, sizeof(Atom)));

  scope_destroy(scope);
  return 0;
}
