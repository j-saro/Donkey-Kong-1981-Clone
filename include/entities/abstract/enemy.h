#pragma once

#include "types.h"
#include "cJSON.h"

void enemy_init(level_t *level, cJSON *json);
void new_enemy(level_t *level, entities_t enemy_type, float pos_x, float pos_y, int direction);
void enemy_destroy(level_t *level, int index);
void enemy_cleanup(level_t *level);
void enemy_draw(cairo_t *cr, const level_t *level);
void enemy_update(level_t *level, float dt_seconds);