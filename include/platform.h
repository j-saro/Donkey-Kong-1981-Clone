#pragma once

#include "cJSON.h"
#include "types.h"

void platform_init(level_t *level, cJSON *platforms_json);
void platform_cleanup(level_t *level);
void platform_draw(cairo_t *cr, const level_t *level);