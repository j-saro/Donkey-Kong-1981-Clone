#pragma once

#include <gtk/gtk.h>
#include "types.h"

void sprites_load_player(gpointer user_data);
void sprites_load_level(gpointer user_data);
void sprites_cleanup(game_state_t *game_state);