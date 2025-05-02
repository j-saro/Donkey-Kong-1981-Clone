#pragma once

#include <gtk/gtk.h>
#include "types.h"


void update_animation_progress(entity_t *base, float dt_seconds);
void update_animation_frame(entity_t *base);
void set_animation(entity_t *base, animation_state_t new_anim);