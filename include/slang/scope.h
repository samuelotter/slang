#ifndef _SCOPE_H_
#define _SCOPE_H_

#include <memory.h>

#include "type.h"
#include "ref.h"

/* Constants ******************************************************************/

#define BLOCK_SIZE 4096

/* Types **********************************************************************/

struct Scope;
struct Block;

typedef int (*Destructor)(void*);

typedef struct Block {
  char           type;
  struct Block   *prev, *next;
  struct Scope   *scope;
  void           *end;
  void           *head;
} Block;

typedef struct Scope {
  Block     *free_blocks;
} Scope;

/* Macros *********************************************************************/

#define scopeof(REF) \
  (((Block*)((void*)REF - ((size_t)REF % BLOCK_SIZE)))->scope)

#define scope_new(SCOPE, TYPE) ((TYPE*)scope_alloc(SCOPE, typeinfo(TYPE)))

/* API ************************************************************************/

Scope *scope_begin();
void   scope_destroy(Scope* scope);

void   *scope_alloc(Scope *scope, size_t size);
Ref     scope_alloc_ref(Scope *scope, size_t size, TypeId type, int ref_data);

#endif /* _SCOPE_H_ */
