#include <stdio.h>
#include <stdlib.h>
#include "entities/abstract/geometry.h"
#include "core/sprite.h"
#include <gtk/gtk.h>
#include "core/sprite.h"

void geometry_parse(geometry_t *structure, cJSON *json, entities_t type);
void geometry_array_cleanup(geometry_t **array, int *count);
void geometry_draw(cairo_t *cr, const geometry_t *array, int count);

void geometry_parse(geometry_t *structure, cJSON *json, entities_t type) {
    structure->type = type;

    structure->x = (float)cJSON_GetObjectItem(json, "x")->valuedouble;
    structure->y = (float)cJSON_GetObjectItem(json, "y")->valuedouble;
    structure->width = (float)cJSON_GetObjectItem(json, "width")->valuedouble;
    structure->height = (float)cJSON_GetObjectItem(json, "height")->valuedouble;

    cJSON *physics = cJSON_GetObjectItem(json, "has_physics");
    structure->has_physics = (physics == NULL) ? true : cJSON_IsTrue(physics);
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

        cairo_surface_t *frame_surface = sprite_sheets[array->type];
        if (frame_surface != NULL) { 
            cairo_pattern_t *pattern = cairo_pattern_create_for_surface(frame_surface);
            cairo_pattern_set_extend(pattern, CAIRO_EXTEND_REPEAT);

            cairo_matrix_t matrix;
            cairo_matrix_init_translate(&matrix, -structure->x, -structure->y);
            cairo_pattern_set_matrix(pattern, &matrix);

            cairo_set_source(cr, pattern);
            cairo_rectangle(cr, structure->x, structure->y, structure->width, structure->height);
            cairo_fill(cr);

            cairo_pattern_destroy(pattern);
        } else {
            g_warning("Invalid frame surface for geometry");
        }
        cairo_restore(cr);
    }
}