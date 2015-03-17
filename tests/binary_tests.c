#include "test_util.h"

#include <string.h>

#include <binary.h>
#include <crc32.h>

test_suite(binary_tests, {
    Scope *scope = scope_begin();
    const char *str = "Hello binary world!";

    test_case(create_binary, {
        Binary *b       = binary(scope, (const uint8_t*)str, sizeof(str));
        assert((void*)str != (void*)b->data);
        assert(strcmp(str, (const char*)b->data) == 0);
      });

    test_case(hash_binary, {
        Binary *b     = binary(scope, (const uint8_t*)str, sizeof(str));
        uint32_t hash = crc32(0, str, sizeof(str));
        assert(hash == binary_hash32(0, b));
      });

    scope_destroy(scope);
  });
