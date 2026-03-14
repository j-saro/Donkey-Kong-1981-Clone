#pragma once

#include <gtk/gtk.h>
#include "game.h"

gboolean on_key_pressed(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
gboolean on_key_released(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
gboolean on_focus_out(GtkWidget *widget, GdkEventFocus *event, gpointer user_data);
void input_init(game_state_t *game_state);
void input_cleanup(game_state_t *game_state);
gboolean is_key_pressed(game_state_t *game_state, unsigned int keyval);