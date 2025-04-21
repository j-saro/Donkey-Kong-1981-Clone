#include <stdio.h>
#include <stdlib.h>
#include "platform.h"

void platform_init(level_t *level, cJSON *platforms_json);
void platform_cleanup(level_t *level);
void platform_draw(cairo_t *cr, const level_t *level);

void platform_init(level_t *level, cJSON *platforms_json) {
    level->num_platforms = cJSON_GetArraySize(platforms_json);
    level->platforms = malloc(level->num_platforms * sizeof(structure_t));

    for (int i = 0; i < level->num_platforms; i++) {
        cJSON *platform_json = cJSON_GetArrayItem(platforms_json, i);

        // Extract platform attributes (x, y, width, height)
        level->platforms[i].x = (float)cJSON_GetObjectItem(platform_json, "x")->valuedouble;
        level->platforms[i].y = (float)cJSON_GetObjectItem(platform_json, "y")->valuedouble;
        level->platforms[i].width = (float)cJSON_GetObjectItem(platform_json, "width")->valuedouble;
        level->platforms[i].height = (float)cJSON_GetObjectItem(platform_json, "height")->valuedouble;
        cJSON *physics = cJSON_GetObjectItem(platform_json, "has_physics");
        if (physics == NULL) {
            level->platforms[i].has_physics = true;
        } else {
            level->platforms[i].has_physics = cJSON_IsTrue(physics);
        }
    }
}

void platform_cleanup(level_t *level) {
    free(level->platforms);
    level->platforms = NULL;
    level->num_platforms = 0;
}

void platform_draw(cairo_t *cr, const level_t *level) {
    cairo_surface_t *surface = level->platform_sprite_sheet;
    
    for (int i = 0; i < level->num_platforms; ++i) {
        const structure_t *platform = &level->platforms[i];

        cairo_save(cr);

        cairo_pattern_t *pattern = cairo_pattern_create_for_surface(surface);
        cairo_pattern_set_extend(pattern, CAIRO_EXTEND_REPEAT);

        cairo_matrix_t matrix;
        cairo_matrix_init_translate(&matrix, -platform->x, -platform->y);
        cairo_pattern_set_matrix(pattern, &matrix);

        cairo_set_source(cr, pattern);
        cairo_rectangle(cr, 
                        platform->x, 
                        platform->y,
                        platform->width, 
                        platform->height);
        cairo_fill(cr);

        cairo_pattern_destroy(pattern);
        cairo_restore(cr);
    }
}