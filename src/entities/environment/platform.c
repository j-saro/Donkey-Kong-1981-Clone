#include <stdio.h>
#include <stdlib.h>
#include "entities/environment/platform.h"
#include "entities/abstract/geometry.h"
#include <gtk/gtk.h>

void platform_init(level_t *level, cJSON *platforms_json);
void platform_cleanup(level_t *level);
void platform_draw(cairo_t *cr, game_state_t *game_state);

void platform_init(level_t *level, cJSON *platforms_json) {
    level->num_platforms = cJSON_GetArraySize(platforms_json);
    level->platforms = malloc(level->num_platforms * sizeof(geometry_t));

    for (int i = 0; i < level->num_platforms; i++) {
        cJSON *platform_json = cJSON_GetArrayItem(platforms_json, i);
        geometry_parse(&level->platforms[i], platform_json, PLATFORM);
    }
}

void platform_cleanup(level_t *level) {
    // Cleanup platforms array
    geometry_array_cleanup(&level->platforms, &level->num_platforms);
}

void platform_draw(cairo_t *cr, game_state_t *game_state) {
    level_t *level = &game_state->level;
    geometry_draw(cr, level->platforms, level->num_platforms, game_state);
}