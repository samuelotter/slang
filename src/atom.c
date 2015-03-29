#include "atom.h"
#include "crc32.h"

#include <string.h>
#include <stdlib.h>
#include <assert.h>

deftype(Atom);

/* Data ***********************************************************************/

#define ATOM_TABLE_BUCKETS 1024

static Atom* atom_table[ATOM_TABLE_BUCKETS] = { 0 };

static _Bool lookup(const char* name, Atom** atom);
static void  insert(Atom* atom);
static Atom *make_atom(const char *str, size_t len);

/* API ************************************************************************/

Atom* atom(const char* name) {
  size_t name_len = strnlen(name, ATOM_MAX_LENGTH);
  return make_atom(name, name_len);
}

Atom *atom_from_binary(Binary *binary) {
  return make_atom((char *)binary->data, binary->size);
}

uint32_t atom_hash32(uint32_t hash, Atom *atom) {
  return crc32(hash, atom->name, strlen(atom->name));
}

/* Internal functions *********************************************************/

Atom *make_atom(const char *name, size_t len) {
  assert(name != NULL);
  assert(len <= ATOM_MAX_LENGTH);
  Atom* atom = NULL;
  if (lookup(name, &atom)) {
    return atom;
  }
  atom         = (Atom*)malloc(sizeof(Atom));
  atom->hash   = crc32(0, name, strlen(name));
  atom->next   = NULL;
  atom->header = ref_header(TYPEID_ATOM, 0);
  memset(atom->name, 0, ATOM_MAX_LENGTH + 1);
  memcpy(atom->name, name, len);
  atom->name[len] = 0;
  insert(atom);
  return atom;
}

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
