#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <scope.h>
#include <binary.h>
#include <syntax.h>
#include <list.h>

Binary *read_file(Scope *scope, const char *filename) {
  // TODO some kind of error handling...
  FILE *f        = fopen(filename, "r");
  fseek(f, 0, SEEK_END);
  size_t size    = ftell(f);
  fseek(f, 0, SEEK_SET);
  Binary *binary = binary_new(scope, size);
  size_t read    = fread(binary->data, sizeof(char), size, f);
  assert(read == size);
  fclose(f);
  return binary;
}

void pretty_print(List *list) {
  printf("(");
  while (!list_is_empty(list)) {
    if (ref_type(list->value) == TYPEID_LIST) {
      pretty_print(list->value.list);
    } else if (ref_type(list->value) == TYPEID_BINARY) {
      printf("%s ", list->value.binary->data);
    } else {
      printf("???");
    }
    list = list->next;
  }
  printf(")");
}

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Missing filename");
    exit(1);
  }
  char *filename = argv[1];

  Scope *scope    = scope_begin();
  Binary *content = read_file(scope, filename);
  List *tokens    = syntax_tokenize(content);
  //  pretty_print(tokens);
  List *expr      = syntax_parse(tokens);
  pretty_print(expr);
  assert(expr != NULL);
  //ref_print(expr);

  scope_destroy(scope);
  return 0;
}
