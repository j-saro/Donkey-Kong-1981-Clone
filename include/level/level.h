#pragma once

#include <gtk/gtk.h>
#include "types.h"

gboolean level_load(game_state_t *game_state);
gboolean level_next(game_state_t *game_state);
void level_cleanup(level_t *level);
void level_draw(cairo_t *cr, game_state_t *game_state);
void level_update(game_state_t *game_state, float dt_seconds);