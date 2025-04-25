#pragma once

#include "types.h"
#include "cJSON.h"


extern cairo_surface_t ****animation_frame_sprites;
extern cairo_surface_t **sprite_sheets;
extern animation_sequence_t *animations;

void sprite_init(cJSON *sprite_sheet_json, cJSON *animation_json);
void sprite_cleanup();
int get_type_by_name(const char *name);