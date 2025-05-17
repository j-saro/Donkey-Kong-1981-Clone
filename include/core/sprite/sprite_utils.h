#pragma once

#include "types.h"
#include <gtk/gtk.h>

GHashTable* get_string_to_enum_table();
animation_sequence_t get_animation_by_key(entity_t *base, animation_state_t anim_state);
void set_animation_frames(entity_t *base);
cairo_surface_t *get_spritesheet(entities_t entity);
int get_type_by_name(const char *name);