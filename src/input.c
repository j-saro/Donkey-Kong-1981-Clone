#include "input.h"

gboolean on_key_pressed(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    game_state_t *game_state = (game_state_t*) user_data;
    if (event->keyval < game_state->num_pressed_keys) {
        game_state->pressed_keys[event->keyval] = 1;
    }
    return TRUE;
}

gboolean on_key_released(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    game_state_t *game_state = (game_state_t*) user_data;
    if (event->keyval < game_state->num_pressed_keys) {
        game_state->pressed_keys[event->keyval] = 0;
    }
    return TRUE;
}
