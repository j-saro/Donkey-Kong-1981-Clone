#include "core/input.h"


gboolean on_key_pressed(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
gboolean on_key_released(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
gboolean on_focus_out(GtkWidget *widget, GdkEventFocus *event, gpointer user_data);
void input_init(game_state_t *game_state);
void input_cleanup(game_state_t *game_state);
gboolean is_key_pressed(game_state_t *game_state, unsigned int keyval);

gboolean on_key_pressed(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    game_state_t *game_state = (game_state_t*) user_data;
    if (!game_state || !game_state->pressed_keys) {
        g_warning("Input system not initialized or invalid game state for key press event.");
        return FALSE;
    }

    guint keyval = gdk_keyval_to_lower(event->keyval);

    // Insert keyval into hash table (gpointer keyval & gpointer int 1)
    g_hash_table_insert(game_state->pressed_keys, GINT_TO_POINTER(keyval), GINT_TO_POINTER(1));
    return TRUE;
}

gboolean on_key_released(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    game_state_t *game_state = (game_state_t*) user_data;
    if (!game_state || !game_state->pressed_keys) {
        g_warning("Input system not initialized or invalid game state for key release event.");
        return FALSE;
    }

    guint keyval = gdk_keyval_to_lower(event->keyval);

    // Remove keyval from hash table
    g_hash_table_remove(game_state->pressed_keys, GINT_TO_POINTER(keyval));
    return TRUE;
}

gboolean on_focus_out(GtkWidget *widget, GdkEventFocus *event, gpointer user_data) {
    game_state_t *game_state = (game_state_t*) user_data;
    g_hash_table_remove_all(game_state->pressed_keys);
    return FALSE;
}

void input_init(game_state_t *game_state) {
    // gpointer keyval direct as key
    game_state->pressed_keys = g_hash_table_new(g_direct_hash, g_direct_equal);
    if (!game_state->pressed_keys) {
        g_warning("Failed to create GHashTable for key states!");
    }
}

void input_cleanup(game_state_t *game_state) {
    if (game_state->pressed_keys) {
        // Destroy the hash table and free its memory.
        g_hash_table_destroy(game_state->pressed_keys);
        game_state->pressed_keys = NULL;
    }
}

gboolean is_key_pressed(game_state_t *game_state, unsigned int keyval) {
    if (!game_state || !game_state->pressed_keys) {
        return FALSE;
    }
    // True, if keyval in hash tabel, else False
    return g_hash_table_lookup(game_state->pressed_keys, GINT_TO_POINTER(keyval)) != NULL;
}