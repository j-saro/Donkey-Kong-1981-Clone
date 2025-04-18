#pragma once

#include <gtk/gtk.h>
#include "game.h"

gboolean on_key_pressed(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
gboolean on_key_released(GtkWidget *widget, GdkEventKey *event, gpointer user_data);

