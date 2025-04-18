#pragma once

#include <gtk/gtk.h>
#include "types.h"

void player_init(player_t *player);
void player_draw(cairo_t *cr, const player_t *player);
void player_movement(game_state_t *game_state, float dt_seconds, float screen_width);
