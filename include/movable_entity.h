#pragma once

#include "types.h"

void movable_entitiy_load_sprites(movable_entity_t *base, const char *spritesheet);
void movable_entitiy_cleanup(movable_entity_t *base);
void movable_entitiy_draw(cairo_t *cr, movable_entity_t *base);