#include "atom.h"
#include "crc32.h"

#include <string.h>
#include <stdlib.h>
#include <assert.h>

deftype(Atom);

/* Data ***********************************************************************/

#define ATOM_TABLE_BUCKETS 1024

static Atom* atom_table[ATOM_TABLE_BUCKETS] = { 0 };

_Bool lookup(const char* name, Atom** atom);
void  insert(Atom* atom);

/* API ************************************************************************/

Atom* atom(const char* name) {
  Atom* atom = NULL;
  if (lookup(name, &atom)) {
    return atom;
  }
  size_t name_len = strnlen(name, ATOM_MAX_LENGTH);
  assert(name_len <= ATOM_MAX_LENGTH);

  atom = (Atom*)malloc(sizeof(Atom));
  atom->hash   = crc32(0, name, strlen(name));
  atom->next   = NULL;
  atom->header = ref_header(TYPEID_ATOM, 0);
  memset(atom->name, 0, ATOM_MAX_LENGTH + 1);
  memcpy(atom->name, name, name_len);
  atom->name[name_len] = 0;
  insert(atom);
  return atom;
}

uint32_t atom_hash32(uint32_t hash, Atom *atom) {
  return crc32(hash, atom->name, strlen(atom->name));
}

/* Internal functions *********************************************************/

_Bool lookup(const char* name, Atom** atom) {
  uint32_t hash = crc32(0, name, strlen(name));
  *atom         = atom_table[hash % ATOM_TABLE_BUCKETS];
  while (*atom != NULL) {
    if (strncmp(name, (*atom)->name, ATOM_MAX_LENGTH) == 0) {
      return 1;
    }
    *atom = (*atom)->next;
  }
  return 0;
}

void insert(Atom* atom) {
  Atom* head = atom_table[atom->hash % ATOM_TABLE_BUCKETS];
  if (head == NULL) {
    atom_table[atom->hash % ATOM_TABLE_BUCKETS] = atom;
    return;
  }
  while (head->next != NULL) {
    head = head->next;
  }
  head->next = atom;
}
