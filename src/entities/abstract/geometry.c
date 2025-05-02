#include <stdio.h>
#include <stdlib.h>
#include "entities/abstract/geometry.h"
#include "core/sprite/sprite.h"
#include <gtk/gtk.h>
#include "core/sprite/sprite_utils.h"
#include "entities/abstract/entity.h"

void geometry_parse(geometry_t *structure, cJSON *json, entities_t type);
void geometry_array_cleanup(geometry_t **array, int *count);
void geometry_draw(cairo_t *cr, geometry_t *array, int count, game_state_t *game_state);

void geometry_parse(geometry_t *structure, cJSON *json, entities_t type) {
    structure->base.type = type;

    structure->base.x = (float)cJSON_GetObjectItem(json, "x")->valuedouble;
    structure->base.y = (float)cJSON_GetObjectItem(json, "y")->valuedouble;
    structure->base.width = (float)cJSON_GetObjectItem(json, "width")->valuedouble;
    structure->base.height = (float)cJSON_GetObjectItem(json, "height")->valuedouble;

    cJSON *physics = cJSON_GetObjectItem(json, "has_physics");
    structure->has_physics = (physics == NULL) ? true : cJSON_IsTrue(physics);

    cJSON *cutscene = cJSON_GetObjectItem(json, "is_cutscene_entity");
    structure->is_cutscene_entity = (cutscene == NULL) ? false : cJSON_IsTrue(cutscene);

    cJSON *cutscene_id = cJSON_GetObjectItem(json, "cutscene_id");
    structure->cutscene_id = (cJSON_IsNumber(cutscene_id)) ? cutscene_id->valueint : -1;
    
    cairo_surface_t *surface = get_spritesheet(structure->base.type);
    if (surface != NULL) {
        structure->base.animation.current_frame = surface;
    } else {
        structure->base.animation.current_frame = NULL;
        g_warning("No geometry spritesheet found for %d", structure->base.type);
    }
    
}

void geometry_array_cleanup(geometry_t **array, int *count) {
    free(*array);
    *array = NULL;
    *count = 0;
}

void geometry_draw(cairo_t *cr, geometry_t *array, int count, game_state_t *game_state) {
    for (int i = 0; i < count; ++i) {
        const geometry_t *structure = &array[i];

        // Skip cutscene-only entities if we're not in cutscene mode
        if (game_state->mode != GAME_MODE_CUTSCENE) {
            if (structure->is_cutscene_entity)
                continue;
        }
        else {
            // We're in cutscene mode
            if (structure->is_cutscene_entity) {
                // Only show this entity if its cutscene_id matches the current cutscene
                if (structure->cutscene_id != game_state->current_cutscene)
                    continue;
            }
            else {
                // Optionally hide certain non-cutscene entities during specific cutscenes
                if (game_state->current_cutscene == 1 &&
                    structure->base.type == LADDER)
                {
                    continue;
                }
            }
        }

        cairo_save(cr);

        cairo_surface_t *frame_surface = array->base.animation.current_frame;
        if (frame_surface != NULL) { 
            cairo_pattern_t *pattern = cairo_pattern_create_for_surface(frame_surface);
            cairo_pattern_set_extend(pattern, CAIRO_EXTEND_REPEAT);
            cairo_pattern_set_filter(pattern, CAIRO_FILTER_NEAREST);

            cairo_matrix_t matrix;
            cairo_matrix_init_translate(&matrix, -structure->base.x, -structure->base.y);
            cairo_pattern_set_matrix(pattern, &matrix);

            cairo_set_source(cr, pattern);
            cairo_rectangle(cr, structure->base.x, structure->base.y, structure->base.width, structure->base.height);
            cairo_fill(cr);

            cairo_pattern_destroy(pattern);
        } else {
            g_warning("Invalid frame surface for geometry");
        }
        cairo_restore(cr);
    }
}