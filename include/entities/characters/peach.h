#pragma once

#include <gtk/gtk.h>
#include "types.h"
#include "cJSON.h"


void peach_init(peach_t *peach, cJSON *json);
void peach_cleanup(movable_entity_t *base);
void peach_draw(cairo_t *cr, const movable_entity_t *base);
void peach_update(peach_t *peach, float dt_seconds);