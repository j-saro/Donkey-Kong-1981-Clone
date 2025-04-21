#pragma once

#include "cJSON.h"
#include "types.h"

void barrel_init(level_t *level, cJSON *barrels_json);
void barrel_cleanup(level_t *level);
void barrel_draw(cairo_t *cr, const level_t *level);