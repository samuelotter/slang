#ifndef _ATOM_H_
#define _ATOM_H_

#include <stddef.h>

#define ATOM_MAX_LENGTH 255

// Typedefs --------------------------------------------------------------------

typedef struct Atom {
  size_t       hash;
  struct Atom* next;
  char         name[ATOM_MAX_LENGTH + 1];
} Atom;

// API -------------------------------------------------------------------------

Atom* atom(const char* name);

#endif
