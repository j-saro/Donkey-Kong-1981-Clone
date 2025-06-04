#pragma once

#include "types.h"
#include <gtk/gtk.h>

gboolean sprite_load_from_json(game_state_t *game_state, const char *filename);
gboolean level_load_from_json(level_t *level, const char *filename);