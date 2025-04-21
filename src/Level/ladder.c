#include <stdio.h>
#include <stdlib.h>
#include "ladder.h"
#include "structure.h"

void ladder_init(level_t *level, cJSON *ladders_json);
void ladder_cleanup(level_t *level);
void ladder_draw(cairo_t *cr, const level_t *level);

void ladder_init(level_t *level, cJSON *ladders_json) {
    level->num_ladders = cJSON_GetArraySize(ladders_json);
    level->ladders = malloc(level->num_ladders * sizeof(structure_t));

    for (int i = 0; i < level->num_ladders; i++) {
        cJSON *ladder_json = cJSON_GetArrayItem(ladders_json, i);
        structure_parse(&level->ladders[i], ladder_json);
    }
}

void ladder_cleanup(level_t *level) {
    structure_cleanup(&level->ladders, &level->num_ladders);
}


void ladder_draw(cairo_t *cr, const level_t *level) {
    structure_draw(cr, level->ladders, level->num_ladders, level->ladder_sprite_sheet);
}