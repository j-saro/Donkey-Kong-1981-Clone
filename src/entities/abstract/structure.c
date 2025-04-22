#include <stdio.h>
#include <stdlib.h>
#include "entities/abstract/structure.h"

void structure_parse(structure_t *structure, cJSON *json);
void structure_array_cleanup(structure_t **array, int *count);
void structure_draw(cairo_t *cr, const structure_t *array, int count, cairo_surface_t *surface);

void structure_parse(structure_t *structure, cJSON *json) {
    structure->x = (float)cJSON_GetObjectItem(json, "x")->valuedouble;
    structure->y = (float)cJSON_GetObjectItem(json, "y")->valuedouble;
    structure->width = (float)cJSON_GetObjectItem(json, "width")->valuedouble;
    structure->height = (float)cJSON_GetObjectItem(json, "height")->valuedouble;

    cJSON *physics = cJSON_GetObjectItem(json, "has_physics");
    structure->has_physics = (physics == NULL) ? true : cJSON_IsTrue(physics);
}

void structure_array_cleanup(structure_t **array, int *count) {
    free(*array);
    *array = NULL;
    *count = 0;
}

void structure_draw(cairo_t *cr, const structure_t *array, int count, cairo_surface_t *surface) {
    for (int i = 0; i < count; ++i) {
        const structure_t *structure = &array[i];

        cairo_save(cr);

        cairo_pattern_t *pattern = cairo_pattern_create_for_surface(surface);
        cairo_pattern_set_extend(pattern, CAIRO_EXTEND_REPEAT);

        cairo_matrix_t matrix;
        cairo_matrix_init_translate(&matrix, -structure->x, -structure->y);
        cairo_pattern_set_matrix(pattern, &matrix);

        cairo_set_source(cr, pattern);
        cairo_rectangle(cr, structure->x, structure->y, structure->width, structure->height);
        cairo_fill(cr);

        cairo_pattern_destroy(pattern);
        cairo_restore(cr);
    }
}