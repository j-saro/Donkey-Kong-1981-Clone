#pragma once

#include "cJSON.h"
#include "types.h"

void ladder_init(level_t *level, cJSON *ladders_json);
void ladder_cleanup(level_t *level);
void ladder_draw(cairo_t *cr, const level_t *level);