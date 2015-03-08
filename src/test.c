#include "scope.h"

#include <stdio.h>

int* do_stuff(Scope* scope) {
  int* var = (int*)scope_alloc(scope, sizeof(int));
  *var     = 42;
  return var;
}

int main(int argc, char** argv) {
  Scope* scope = scope_new();

  int* var = do_stuff(scope);
  printf("var = %d\n", *var);

  scope_destroy(scope);
}
