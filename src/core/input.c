#include "core/input.h"


gboolean on_key_pressed(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
gboolean on_key_released(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
void input_init(game_state_t *game_state);
void input_cleanup(game_state_t *game_state);

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

void input_init(game_state_t *game_state) {
    game_state->num_pressed_keys = 256; // 65536 for most of keyval 
    game_state->pressed_keys = calloc(game_state->num_pressed_keys, sizeof(int));
}

void input_cleanup(game_state_t *game_state) {
    free(game_state->pressed_keys);
    game_state->pressed_keys = NULL;
    game_state->num_pressed_keys = 0;
}