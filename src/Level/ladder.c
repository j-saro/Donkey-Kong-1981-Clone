#include <stdio.h>
#include <stdlib.h>
#include "ladder.h"

void ladder_init(level_t *level, cJSON *ladders_json);
void ladder_cleanup(level_t *level);
void ladder_draw(cairo_t *cr, const level_t *level);

void ladder_init(level_t *level, cJSON *ladders_json) {
    level->num_ladders = cJSON_GetArraySize(ladders_json);
    level->ladders = malloc(level->num_ladders * sizeof(structure_t));

    for (int i = 0; i < level->num_ladders; i++) {
        cJSON *ladder_json = cJSON_GetArrayItem(ladders_json, i);

        // Extract ladder attributes (x, y, width, height)
        level->ladders[i].position.x = (float)cJSON_GetObjectItem(ladder_json, "x")->valuedouble;
        level->ladders[i].position.y = (float)cJSON_GetObjectItem(ladder_json, "y")->valuedouble;
        level->ladders[i].width = (float)cJSON_GetObjectItem(ladder_json, "width")->valuedouble;
        level->ladders[i].height = (float)cJSON_GetObjectItem(ladder_json, "height")->valuedouble;
    }
}

void ladder_cleanup(level_t *level) {
    free(level->ladders);
    level->ladders = NULL;
    level->num_ladders = 0;
}

void ladder_draw(cairo_t *cr, const level_t *level) {
    cairo_surface_t *surface = level->ladder_sprite_sheet;

    for (int i = 0; i < level->num_ladders; ++i) {
        const structure_t *ladder = &level->ladders[i];

        cairo_save(cr);

        cairo_pattern_t *pattern = cairo_pattern_create_for_surface(surface);
        cairo_pattern_set_extend(pattern, CAIRO_EXTEND_REPEAT);

        cairo_matrix_t matrix;
        cairo_matrix_init_translate(&matrix, -ladder->position.x, -ladder->position.y);
        cairo_pattern_set_matrix(pattern, &matrix);

        cairo_set_source(cr, pattern);
        cairo_rectangle(cr, 
                        ladder->position.x, 
                        ladder->position.y,
                        ladder->width, 
                        ladder->height);
        cairo_fill(cr);

        cairo_pattern_destroy(pattern);
        cairo_restore(cr);
    }
}