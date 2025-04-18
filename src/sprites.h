#pragma once

#include <gtk/gtk.h>
#include "types.h"

void load_sprites(gpointer user_data);
void cleanup_sprites(player_t *player);
void update_player_animation_state(game_state_t *game_state, float dt_seconds);
void update_player_animation(player_t *player, float dt_seconds);
void set_player_animation(player_t *player, AnimationState new_anim);