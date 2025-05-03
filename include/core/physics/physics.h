#pragma once

#include "types.h"

void apply_physics(game_state_t *game_state, float dt_seconds, float screen_height);
void apply_gravity(entity_t *base, float dt_seconds, float gravity_force);
void entity_jump(entity_t *base, float jump_force);
void check_ladder_collision(game_state_t *game_state);