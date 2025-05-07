#pragma once

#include "types.h"
#include "cJSON.h"

void item_init(level_t *level, cJSON *json);
void item_destroy(level_t *level, int index);
void item_draw(cairo_t *cr, const level_t *level);
void item_cleanup(level_t *level);