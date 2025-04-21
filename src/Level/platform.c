#include <stdio.h>
#include <stdlib.h>
#include "platform.h"
#include "structure.h"
#include <gtk/gtk.h>

void platform_init(level_t *level, cJSON *platforms_json);
void platform_load_sprite(level_t *level);
void platform_cleanup(level_t *level);
void platform_draw(cairo_t *cr, const level_t *level);

void platform_init(level_t *level, cJSON *platforms_json) {
    platform_load_sprite(level);

    level->num_platforms = cJSON_GetArraySize(platforms_json);
    level->platforms = malloc(level->num_platforms * sizeof(structure_t));

    for (int i = 0; i < level->num_platforms; i++) {
        cJSON *platform_json = cJSON_GetArrayItem(platforms_json, i);
        structure_parse(&level->platforms[i], platform_json);
    }
}

void platform_load_sprite(level_t *level) {
    level->platform_sprite_sheet = cairo_image_surface_create_from_png("./assets/platform.png");
    if (cairo_surface_status(level->platform_sprite_sheet) != CAIRO_STATUS_SUCCESS) {
        g_warning("Failed to create platform spritesheet");
        return;
    }
}

void platform_cleanup(level_t *level) {
    // Cleanup Platform Sheet
    if (level->platform_sprite_sheet != NULL) {
        cairo_surface_destroy(level->platform_sprite_sheet);
        level->platform_sprite_sheet = NULL;
    }

    // Cleanup platforms array
    structure_array_cleanup(&level->platforms, &level->num_platforms);
}

void platform_draw(cairo_t *cr, const level_t *level) {
    structure_draw(cr, level->platforms, level->num_platforms, level->platform_sprite_sheet);
}