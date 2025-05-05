#pragma once

#include "types.h"

bool player_object_collision(player_t *player, entity_t *base, float object_width, float object_heigth);
void entity_jump(entity_t *base, float jump_force);