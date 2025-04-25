#pragma once

#include "cJSON.h"
#include "types.h"


void geometry_parse(geometry_t *structure, cJSON *json, entities_t type);
void geometry_array_cleanup(geometry_t **array, int *count);
void geometry_draw(cairo_t *cr, const geometry_t *array, int count);