#pragma once

#include "cJSON.h"
#include "types.h"

void static_entity_init(level_t *level, cJSON *static_entities_json);
void static_entity_cleanup(level_t *level);
void hide_static_entity(level_t *level);
void show_static_entity(level_t *level);
void static_entity_draw(cairo_t *cr, const level_t *level);