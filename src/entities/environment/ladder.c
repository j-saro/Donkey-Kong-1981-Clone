#include <stdio.h>
#include <stdlib.h>
#include "entities/environment/ladder.h"
#include "entities/abstract/geometry.h"
#include <gtk/gtk.h>

void ladder_init(level_t *level, cJSON *ladders_json);
void ladder_cleanup(level_t *level);
void ladder_draw(cairo_t *cr, const level_t *level);

void ladder_init(level_t *level, cJSON *ladders_json) {
    level->num_ladders = cJSON_GetArraySize(ladders_json);
    level->ladders = malloc(level->num_ladders * sizeof(geometry_t));

    for (int i = 0; i < level->num_ladders; i++) {
        cJSON *ladder_json = cJSON_GetArrayItem(ladders_json, i);
        geometry_parse(&level->ladders[i], ladder_json, LADDER);
    }
}

void ladder_cleanup(level_t *level) {
    // Cleanup ladders array
    geometry_array_cleanup(&level->ladders, &level->num_ladders);
}


void ladder_draw(cairo_t *cr, const level_t *level) {
    geometry_draw(cr, level->ladders, level->num_ladders);
}