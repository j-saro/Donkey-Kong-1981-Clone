#include <stdio.h>
#include <stdlib.h>
#include "platform.h"
#include "structure.h"

void platform_init(level_t *level, cJSON *platforms_json);
void platform_cleanup(level_t *level);
void platform_draw(cairo_t *cr, const level_t *level);

void platform_init(level_t *level, cJSON *platforms_json) {
    level->num_platforms = cJSON_GetArraySize(platforms_json);
    level->platforms = malloc(level->num_platforms * sizeof(structure_t));

    for (int i = 0; i < level->num_platforms; i++) {
        cJSON *platform_json = cJSON_GetArrayItem(platforms_json, i);
        structure_parse(&level->platforms[i], platform_json);
    }
}

void platform_cleanup(level_t *level) {
    structure_cleanup(&level->platforms, &level->num_platforms);
}

void platform_draw(cairo_t *cr, const level_t *level) {
    structure_draw(cr, level->platforms, level->num_platforms, level->platform_sprite_sheet);
}