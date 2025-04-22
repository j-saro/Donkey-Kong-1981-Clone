#include <stdio.h>
#include <stdlib.h>
#include "entities/environment/ladder.h"
#include "entities/abstract/structure.h"
#include <gtk/gtk.h>

void ladder_init(level_t *level, cJSON *ladders_json);
void ladder_load_sprite(level_t *level);
void ladder_cleanup(level_t *level);
void ladder_draw(cairo_t *cr, const level_t *level);

void ladder_init(level_t *level, cJSON *ladders_json) {
    ladder_load_sprite(level);

    level->num_ladders = cJSON_GetArraySize(ladders_json);
    level->ladders = malloc(level->num_ladders * sizeof(structure_t));

    for (int i = 0; i < level->num_ladders; i++) {
        cJSON *ladder_json = cJSON_GetArrayItem(ladders_json, i);
        structure_parse(&level->ladders[i], ladder_json);
    }
}

void ladder_load_sprite(level_t *level) {
    level->ladder_sprite_sheet = cairo_image_surface_create_from_png("./assets/ladder.png");
    if (cairo_surface_status(level->ladder_sprite_sheet) != CAIRO_STATUS_SUCCESS) {
        g_warning("Failed to create ladder spritesheet");
        return;
    }
}

void ladder_cleanup(level_t *level) {
    // Cleanup Ladder spritesheet
    if (level->ladder_sprite_sheet != NULL) {
        cairo_surface_destroy(level->ladder_sprite_sheet);
        level->ladder_sprite_sheet = NULL;
    }

    // Cleanup ladders array
    structure_array_cleanup(&level->ladders, &level->num_ladders);
}


void ladder_draw(cairo_t *cr, const level_t *level) {
    structure_draw(cr, level->ladders, level->num_ladders, level->ladder_sprite_sheet);
}