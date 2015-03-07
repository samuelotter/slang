#ifndef _SCOPE_H_
#define _SCOPE_H_

#include <memory.h>

// Types -----------------------------------------------------------------------

typedef int (*Destructor)(void*);

typedef struct {
  char*      name;
  Destructor destructor;
} Type;

typedef struct {
  Type* type;
  struct Scope* scope;
} Reference;

typedef struct Resource {
  struct Resource*  next;
  Reference         reference;
} Resource;

typedef struct {
  Resource* first;
} Scope;

// Macros ----------------------------------------------------------------------

#define scopeof(REF) ((Scope*)(REF - sizeof(Scope*)))

// API -------------------------------------------------------------------------

Scope* scope_new();
void   scope_destroy(Scope* scope);

void*  scope_alloc(Scope* scope, size_t size);
void*  scope_free(Scope* scope, Resource* resource);

#endif /* _SCOPE_H_ */
