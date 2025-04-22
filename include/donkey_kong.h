#pragma once

#include <gtk/gtk.h>
#include "types.h"


void donkey_kong_init(donkey_kong_t *donkey_kong);
void donkey_kong_cleanup(movable_entity_t *base);
void donkey_kong_draw(cairo_t *cr, movable_entity_t *base);
void donkey_kong_update(donkey_kong_t *donkey_kong, float dt_seconds);