#ifndef _SCOPE_H_
#define _SCOPE_H_

#include <memory.h>

/* Constants ******************************************************************/

#define PAGE_SIZE  4096
#define BLOCK_SIZE 4096

// Types -----------------------------------------------------------------------

struct Scope;
struct Block;

typedef int (*Destructor)(void*);

typedef struct {
  char*      name;
  Destructor destructor;
} Type;

typedef struct Block {
  char           type;
  struct Block   *prev, *next;
  struct Scope   *scope;
  void           *end;
  void           *head;
} Block;

typedef void* Ref;

typedef struct Scope {
  Block     *free_blocks;
} Scope;

// Macros ----------------------------------------------------------------------

#define scopeof(REF) (((Block*)((void*)REF - ((size_t)REF % PAGE_SIZE)))->scope)

// API -------------------------------------------------------------------------

Scope* scope_new();
void   scope_destroy(Scope* scope);

Ref    scope_alloc(Scope* scope, size_t size);

#endif /* _SCOPE_H_ */
