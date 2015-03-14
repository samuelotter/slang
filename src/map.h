#ifndef _MAP_H_
#define _MAP_H_

#include "scope.h"

#include <stdint.h>

#define MAP_WIDTH 16;

/* Types **********************************************************************/

typedef void* Map;

/* API ************************************************************************/

Map  *map(Scope* scope);

Map  *map_insert(Map* map, const void *key, size_t key_size, const void *value);
const void *map_lookup(Map* map, const void *key, size_t key_size);
Map  *map_remove(Map* map, const void *key, size_t key_size);

#endif
