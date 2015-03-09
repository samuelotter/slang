#include "scope.h"
#include "list.h"

#include <stdio.h>

int* do_stuff(Scope* scope) {
  int* var = (int*)scope_alloc(scope, sizeof(int));
  *var     = 42;
  return var;
}

Ref print_val(Ref count, Ref value) {
  int* c = count;
  printf("%d: %p\n", *c, value);
  *c = *c + 1;
  return count;
}

int main(int argc, char** argv) {
  Scope* scope = scope_new();

  List* a = list_new(scope, (Ref)2);
  List* b = list_cons(a, (Ref)1);

  int* var = do_stuff(scope);

  list_foldl(b, var, &print_val);

  printf("var = %d\n", *var);

  scope_destroy(scope);
}
