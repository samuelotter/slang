/**
 * scope.c
 */

#include <stdlib.h>
#include <assert.h>

#include "scope.h"

// Forward decls. --------------------------------------------------------------

Block* alloc_block();

// API -------------------------------------------------------------------------

Scope* scope_new() {
  Scope* scope = (Scope*)malloc(sizeof(Scope));
  scope->free_blocks = NULL;
  return scope;
}

Ref scope_alloc(Scope *scope, size_t size) {
  assert(scope != NULL);
  Block* block = scope->free_blocks;
  if (block == NULL) {
    block = alloc_block(scope, BLOCK_SIZE);
  } else if (size > BLOCK_SIZE - sizeof(Block)) {
    block = alloc_block(scope, size + sizeof(Block));
  } else if (size > block->end - block->head) {
    block = alloc_block(scope, BLOCK_SIZE);
  }
  void *ptr    = block->head;
  block->head += size;
  return ptr;
}

void scope_destroy(Scope* scope) {
  Block* block = scope->free_blocks;
  while(block != NULL) {
    Block* tmp = block->next;
    free(block);
    block = tmp;
  }
  free(scope);
}

// Internal Functions ----------------------------------------------------------

Block* alloc_block(Scope *scope, size_t size) {
  Block* block       = (Block*)aligned_alloc(BLOCK_SIZE, size);
  block->type        = 1;
  block->scope       = scope;
  block->next        = scope->free_blocks;
  scope->free_blocks = block;
  if (block->next != NULL) {
    block->next->prev = block;
  }
  void* start = block;
  block->head = start + sizeof(Block);
  block->end  = start + size;
  return block;
}
