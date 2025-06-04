#pragma once

#include "types.h"

void gui_init(game_state_t *game_state);
void gui_startmenu_draw(cairo_t *cr, game_state_t *game_state);
void gui_game_complete_draw(cairo_t *cr, game_state_t *game_state);
void gui_game_over_draw(cairo_t *cr, game_state_t *game_state);
void gui_game_draw(cairo_t *cr, game_state_t *game_state);
void gui_update(game_state_t *game_state, float dt_seconds);