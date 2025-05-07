#pragma once

#include "types.h"
#include "cJSON.h"

void effect_init(level_t *level);
void new_effect(level_t *level, animation_state_t animation, float pos_x, float pos_y, int direction);
void effect_cleanup(level_t *level);
void effect_draw(cairo_t *cr, const level_t *level);
void effect_update(level_t *level, float dt_seconds);
void effect_destroy(level_t *level, int index);
void effect_clear_all(level_t *level);