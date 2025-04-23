#pragma once

#include "types.h"
#include "cJSON.h"

void movable_entity_load_sprites(movable_entity_t *base, const char *spritesheet);
void movable_entity_parse(movable_entity_t *base, cJSON *json);
void movable_entity_cleanup(animation_t *animation);
void movable_entity_draw(cairo_t *cr, const movable_entity_t *base);