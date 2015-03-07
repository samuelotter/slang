#include "scope.h"
#include "list.h"

#include <stdio.h>

int* do_stuff(Scope* scope) {
  int* var = (int*)scope_alloc(scope, sizeof(int));
  *var     = 42;
  return var;
}

int main(int argc, char** argv) {
  Scope* scope = scope_new();

  List* a = list_new(scope, (void*)1);
  List* b = list_cons(a, (void*)2);

  int* var = do_stuff(scope);
  printf("var = %d, b = %p\n", *var, b->value);

  scope_destroy(scope);
}
