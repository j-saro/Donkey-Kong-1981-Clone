#pragma once

#include <gtk/gtk.h>
#include "types.h"
#include "cJSON.h"

void donkey_kong_init(donkey_kong_t *donkey_kong, cJSON *json);
void donkey_kong_cleanup(movable_entity_t *base);
void donkey_kong_draw(cairo_t *cr, const movable_entity_t *base);
void donkey_kong_update(donkey_kong_t *donkey_kong, float dt_seconds);