#include <binary.h>

#include <assert.h>

#include <crc32.h>

/* API ************************************************************************/

Binary *binary_new(Scope *scope, size_t size) {
  Binary *binary = scope_alloc(scope, sizeof(Binary) + size);
  binary->size   = size;
  binary->header = ref_header(TYPEID_BINARY, 0);
  memset(binary->data, 0, binary->size);
  return binary;
}

Binary *binary(Scope *scope, const uint8_t *data, size_t size) {
  Binary *binary = scope_alloc(scope, sizeof(Binary) + size);
  binary->size   = size;
  binary->header = ref_header(TYPEID_BINARY, 0);
  memcpy(binary->data, data, size);
  return binary;
}

uint32_t binary_hash32(uint32_t hash, Binary *binary) {
  assert(binary != NULL);
  return crc32(hash, binary->data, binary->size);
}
