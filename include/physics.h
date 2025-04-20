#pragma once

#include "types.h"

void apply_physics(game_state_t *game_state, float dt_seconds, float screen_height);
void check_ladder_collision(game_state_t *game_state);