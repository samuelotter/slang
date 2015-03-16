#ifndef _ATOM_H_
#define _ATOM_H_

#include <stddef.h>

#include "type.h"

#define ATOM_MAX_LENGTH 255

// Typedefs --------------------------------------------------------------------

decltype(Atom, struct Atom);

reftype(Atom,
        struct {
          size_t       hash;
          struct Atom *next;
          char         name[ATOM_MAX_LENGTH + 1];
        });

// API -------------------------------------------------------------------------

Atom *atom(const char* name);

#endif
