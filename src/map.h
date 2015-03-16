#ifndef _MAP_H_
#define _MAP_H_

#include <stdint.h>

#include "scope.h"
#include "type.h"
#include "ref.h"

#define MAP_WIDTH 16

/* Types **********************************************************************/

decltype(Map, struct Map);

/* API ************************************************************************/

Map *map(Scope *scope);

Map *map_insert(Map *map, const Ref key, const Ref value);
const Ref map_lookup(Map *map, const Ref key);
Map *map_remove(Map *map, const Ref key);

uint32_t map_hash32(uint32_t, Map *);

#endif
