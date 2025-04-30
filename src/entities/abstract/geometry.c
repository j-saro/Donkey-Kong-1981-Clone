#include <stdio.h>
#include <stdlib.h>
#include "entities/abstract/geometry.h"
#include "core/sprite.h"
#include <gtk/gtk.h>
#include "core/utils.h"

void geometry_parse(geometry_t *structure, cJSON *json, entities_t type);
void geometry_array_cleanup(geometry_t **array, int *count);
void geometry_draw(cairo_t *cr, const geometry_t *array, int count);

void geometry_parse(geometry_t *structure, cJSON *json, entities_t type) {
    structure->base.type = type;

    structure->base.x = (float)cJSON_GetObjectItem(json, "x")->valuedouble;
    structure->base.y = (float)cJSON_GetObjectItem(json, "y")->valuedouble;
    structure->base.width = (float)cJSON_GetObjectItem(json, "width")->valuedouble;
    structure->base.height = (float)cJSON_GetObjectItem(json, "height")->valuedouble;

    cJSON *physics = cJSON_GetObjectItem(json, "has_physics");
    structure->has_physics = (physics == NULL) ? true : cJSON_IsTrue(physics);

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

void geometry_draw(cairo_t *cr, const geometry_t *array, int count) {
    for (int i = 0; i < count; ++i) {
        const geometry_t *structure = &array[i];

        cairo_save(cr);

        cairo_surface_t *frame_surface = array->base.animation.current_frame;
        if (frame_surface != NULL) { 
            cairo_pattern_t *pattern = cairo_pattern_create_for_surface(frame_surface);
            cairo_pattern_set_extend(pattern, CAIRO_EXTEND_REPEAT);

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