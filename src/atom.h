#ifndef _ATOM_H_
#define _ATOM_H_

#include <stddef.h>

#include "type.h"
#include "ref.h"

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

uint32_t atom_hash32(uint32_t hash, Atom *atom);

#endif
