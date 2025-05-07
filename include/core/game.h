#pragma once

#include <gtk/gtk.h>
#include "types.h"

void game_init(game_state_t *game_state);
gboolean draw(GtkWidget *drawing_area, cairo_t *cr, gpointer user_data);
gboolean update(GtkWidget *drawing_area, GdkFrameClock *clock, gpointer user_data);

