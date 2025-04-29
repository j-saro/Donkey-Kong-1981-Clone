#pragma once

#include "types.h"
#include "cJSON.h"


extern animation_frames_t *loaded_animations;
extern spritesheet_t *sprite_sheets;
extern animation_sequence_t *animations;
extern unsigned int num_animations;
extern unsigned int num_sprite_sheets;

void sprite_init(cJSON *sprite_sheet_json, cJSON *animation_json);
void sprite_cleanup();
cairo_surface_t **get_animation_frames(animation_state_t anim_state);
cairo_surface_t *get_spritesheet(entities_t entity);
int get_type_by_name(const char *name);