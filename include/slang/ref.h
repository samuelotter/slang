#ifndef _REF_H_
#define _REF_H_

#include <stdint.h>

#include "type.h"

typedef enum {
  TYPEID_PTR,
  TYPEID_FLOAT,
  TYPEID_ATOM,
  TYPEID_LIST,
  TYPEID_TUPLE,
  TYPEID_MAP,
  TYPEID_BINARY,
  TYPEID_RECORD
} TypeId;


/* Ref type header consists of a single 32-bit field with the
 * following layout:
 * 32                           4    0
 * +----------------------------+----+
 * | DATA                       |TYPE|
 * +----------------------------+----+
 *
 * TYPE contains the TypeId of the type. The contents of the data
 * segment is type specific.
 */
#define HEADER_TYPE_MASK 0x0F
#define HEADER_TYPE_SIZE 4
#define ref_type(HEADER) ((TypeId)(*(uint32_t*)HEADER & HEADER_TYPE_MASK))
#define ref_data(HEADER) ((uint32_t)(*(uint32_t*)HEADER >> HEADER_TYPE_SIZE))
#define ref_is_type(TYPE, VALUE) ((get_type(*(uint32_t*)VALUE)) == TYPE)
#define ref_header(TYPE, DATA)                                  \
  ((TYPE & HEADER_TYPE_MASK) | (DATA << HEADER_TYPE_SIZE))

#define reftype(TYPE_NAME, TYPE_DEF)               \
  typedef struct TYPE_NAME {                       \
    uint32_t header;                               \
    TYPE_DEF;                                      \
  } TYPE_NAME

typedef union Ref {
  uint32_t      *header;
  struct Atom   *atom;
  struct List   *list;
  struct Tuple  *tuple;
  struct Map    *map;
  struct Binary *binary;
  void          *ptr;
} Ref;

uint32_t ref_hash32(uint32_t, Ref);

#endif
