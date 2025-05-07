#pragma once

#include <gtk/gtk.h>
#include "types.h"
#include "cJSON.h"

void player_init(player_t *player, cJSON *json);
void player_draw(cairo_t *cr, const entity_t *base);
void player_update(game_state_t *game_state, float dt_seconds);
void player_check_death(player_t *player);
void player_movement(game_state_t *game_state, float dt_seconds, float screen_width);