#pragma once

#include "types.h"
#include "cJSON.h"
#include <gtk/gtk.h>

gboolean allocate_new_entity(void **array, unsigned int *num, unsigned int *capacity, size_t type_size);
void init_new_entity_base(entity_t *base, float pos_x, float pos_y, int direction);
void destroy_entity(void *array, unsigned int *count, size_t entity_size, int index);
