#pragma once

#include <gtk/gtk.h>
#include "types.h"
#include "cJSON.h"

extern animation_sequence_t *animations;
void animation_load_form_json(level_t *level, cJSON *json);
void animation_cleanup(void);
void update_animation_progress(animation_t *animation, float dt_seconds);
void update_animation_frame(animation_t *animation);
void set_animation(animation_t *animation, animation_state_t new_anim);