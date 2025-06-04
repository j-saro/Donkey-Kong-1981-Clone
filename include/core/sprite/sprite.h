#pragma once

#include "types.h"
#include "cJSON.h"


extern animation_frames_t *loaded_animations;
extern spritesheet_t *sprite_sheets;
extern animation_sequence_t *animations;
extern unsigned int num_animations;
extern unsigned int num_sprite_sheets;

void sprite_init(game_state_t *game_state, cJSON *sprite_sheet_json, cJSON *animation_json);
void sprite_cleanup();