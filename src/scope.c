/**
 * scope.c
 */

#include <stdlib.h>

#include "scope.h"

// Forward decls. --------------------------------------------------------------

void      scope_append(Scope* scope, Resource* resource);
void      resource_append(Resource* head, Resource* resource);
Resource* destroy_resource(Resource* resource);

// API -------------------------------------------------------------------------

Scope* scope_new() {
  Scope* scope = (Scope*)malloc(sizeof(Scope));
  return scope;
}

Ref scope_alloc(Scope* scope, size_t size) {
  Resource* resource = malloc(sizeof(Resource) + sizeof(size));
  scope_append(scope, resource);
  return resource + sizeof(Resource);
}

void scope_destroy(Scope* scope) {
  Resource* resource = scope->first;
  while(resource != NULL) {
    resource = (Resource*)destroy_resource(resource);
  }
}

// Internal Functions ----------------------------------------------------------

Resource* destroy_resource(Resource* resource) {
  Resource* next = resource->next;
  free(resource);
  return next;
}

void scope_append(Scope* scope, Resource* resource) {
  if (scope->first == NULL) {
    scope->first = resource;
    return;
  } else {
    resource_append(scope->first, resource);
  }
}

void resource_append(Resource* head, Resource* resource) {
  Resource* current = head;
  while (current->next != NULL) {
    current = head->next;
  }
  current->next = resource;
}
