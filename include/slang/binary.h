#ifndef _BINARY_H_
#define _BINARY_H_

#include <stdint.h>

#include <scope.h>
#include <ref.h>

decltype(Binary, struct Binary);
reftype(Binary,
        struct {
          size_t  size;
          uint8_t data[];
        });

Binary  *binary(Scope *scope, const uint8_t *data, size_t size);

uint32_t binary_hash32(uint32_t hash, Binary *binary);

#endif
