#pragma once

#include <gtk/gtk.h>
#include "types.h"

gboolean level_init(level_t *level);
void level_cleanup(level_t *level);
void level_draw(cairo_t *cr, const level_t *level);