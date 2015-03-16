#ifndef _TYPE_H_
#define _TYPE_H_

#include <stddef.h>
#include <stdint.h>

#define TYPE_NAME_MAX 255

typedef struct Type {
  const char   name[TYPE_NAME_MAX + 1];
  const size_t size;
} Type;

#define decltype(NAME, T)                       \
  typedef T NAME;                               \
  extern Type NAME##_t

#define type(NAME) &NAME##_t

#define deftype(TYPE_NAME)                         \
  Type TYPE_NAME##_t = {                           \
    .name = "#TYPE_NAME",                          \
    .size = sizeof(TYPE_NAME)                      \
  }                                                \

#endif
