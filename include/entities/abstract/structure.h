#pragma once

#include "cJSON.h"
#include "types.h"


void structure_parse(structure_t *structure, cJSON *json);
void structure_array_cleanup(structure_t **array, int *count);
void structure_draw(cairo_t *cr, const structure_t *array, int count, cairo_surface_t *surface);