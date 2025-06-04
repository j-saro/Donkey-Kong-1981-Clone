#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include "consts.h"
#include "entities/abstract/geometry.h"
#include "core/sprite/sprite.h"
#include "core/sprite/sprite_utils.h"
#include "entities/abstract/entity.h"

void geometry_parse(geometry_t *structure, cJSON *json, entities_t type);
void geometry_draw(cairo_t *cr, geometry_t *array, int count, game_state_t *game_state);

void geometry_parse(geometry_t *structure, cJSON *json, entities_t type) {
    structure->base.type = type;
    entity_parse(&structure->base, json);
    
    // Get the native dimensions of the animation frame for this geometry
    animation_sequence_t anim_seq = get_animation_by_key(&structure->base, structure->base.animation.current_animation);
    float native_tile_width = (float)anim_seq.frame_width;
    float native_tile_height = (float)anim_seq.frame_height;

    // Gets width and height form JSON
    float json_total_width = (float)cJSON_GetObjectItem(json, "width")->valuedouble;
    float json_total_height = (float)cJSON_GetObjectItem(json, "height")->valuedouble;

    if (type == PLATFORM) {
        structure->base.width = json_total_width;
        structure->base.height = native_tile_height * SCALE;
    } else if (type == LADDER) {
        structure->base.width = native_tile_width * SCALE;
        structure->base.height = json_total_height;
    }

    cJSON *physics_json = cJSON_GetObjectItem(json, "has_physics");
    structure->has_physics = (physics_json == NULL) ? true : cJSON_IsTrue(physics_json);
}

void geometry_draw(cairo_t *cr, geometry_t *array, int count, game_state_t *game_state) {
    for (int i = 0; i < count; ++i) {
        const geometry_t *structure = &array[i];

        cairo_surface_t *frame_surface = structure->base.animation.current_frame;
        if (frame_surface == NULL || structure->base.animation.current_animation == ANIM_HIDE) {
            if (structure->base.animation.current_animation != ANIM_HIDE) {
                 g_warning("Invalid or hidden frame surface for geometry: type %d, anim %d",
                           structure->base.type, structure->base.animation.current_animation);
            }
            continue; 
        }

        cairo_save(cr);

        // Sets the geometry's top left cords as (0,0)
        cairo_translate(cr, structure->base.x, structure->base.y);
        
        cairo_pattern_t *pattern = cairo_pattern_create_for_surface(frame_surface);
        cairo_pattern_set_extend(pattern, CAIRO_EXTEND_REPEAT);
        cairo_pattern_set_filter(pattern, CAIRO_FILTER_NEAREST);

        // Create a transformation matrix
        cairo_matrix_t p_matrix;

        // Initialize the matrix as the identity matrix (no transformation)
        cairo_matrix_init_identity(&p_matrix);

        // If SCALE is not zero, apply an inverse scaling to the matrix
        if (SCALE != 0.0f) {
            // Scale the matrix by 1/SCALE in both X and Y directions (undo a previous scale)
            cairo_matrix_scale(&p_matrix, 1.0f / SCALE, 1.0f / SCALE);
        }

        // Apply the transformation matrix to the Cairo pattern
        cairo_pattern_set_matrix(pattern, &p_matrix);

        // Set the pattern as the source.
        cairo_set_source(cr, pattern);

        // Draw the rectangle
        cairo_rectangle(cr, 0, 0, structure->base.width, structure->base.height);
        cairo_fill(cr);

        cairo_pattern_destroy(pattern);

        cairo_restore(cr);
    }
}