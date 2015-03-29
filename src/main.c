#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <scope.h>
#include <binary.h>
#include <syntax.h>
#include <list.h>
#include <atom.h>

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

void pretty_print(Ref value) {
  // FIXME: no words required
  static int indent = 0;
  if (ref_is_type(TYPEID_LIST, value)) {
    printf("(");
    indent++;
    list_foreach(value.list, pretty_print);
    indent--;
    printf(")");
  } else if (ref_is_type(TYPEID_BINARY, value)) {
    printf("\"%s\"", value.binary->data);
  } else if (ref_is_type(TYPEID_ATOM, value)) {
    printf("'%s'", value.atom->name);
  } else {
    printf("???");
  }
  printf(" ");
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
  assert(tokens != NULL);
  //  pretty_print(tokens);
  Ref  expr       = syntax_parse(tokens);
  pretty_print(expr);
  //ref_print(expr);

  scope_destroy(scope);
  return 0;
}
