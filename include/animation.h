#pragma once

#include <gtk/gtk.h>
#include "types.h"

extern const animation_sequence_t animations[];
void player_change_animation(game_state_t *game_state, float dt_seconds);
void update_animation_progress(animation_t *animation, float dt_seconds);
void update_animation_frame(animation_t *animation);
void set_animation(animation_t *animation, animation_state_t new_anim);