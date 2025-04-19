#include <gtk/gtk.h>
#include <assert.h>

#include "game.h"
#include "input.h"
#include "player.h"
#include "sprites.h"
#include "types.h"
#include "level.h"


static void activate(GtkApplication* app, gpointer user_data);

static void activate(GtkApplication* app, gpointer user_data) {
    game_state_t *game_state = (game_state_t*) user_data;

    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Donkey Kong 1981");

    const unsigned int window_width = 800;
    const unsigned int window_height = 600;

    gtk_window_set_default_size(GTK_WINDOW(window), BASE_WIDTH, BASE_HEIGHT);
    game_state->player.x = BASE_WIDTH * 0.5f;
    game_state->player.y = BASE_HEIGHT * 0.5f;

    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(window), drawing_area);
    gtk_widget_show_all(window);

    g_signal_connect(window, "key_press_event", G_CALLBACK(on_key_pressed), game_state);
    g_signal_connect(window, "key_release_event", G_CALLBACK(on_key_released), game_state);
    g_signal_connect(drawing_area, "draw", G_CALLBACK(draw), game_state);

    gtk_widget_add_tick_callback(drawing_area, update, game_state, NULL);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;
    game_state_t game_state;

    game_state.num_pressed_keys = 256;
    game_state.pressed_keys = calloc(game_state.num_pressed_keys, sizeof(int));

    sprites_load_level(&game_state);
    level_init(&game_state.level);
    player_init(&game_state.player);
    sprites_load_player(&game_state);

    app = gtk_application_new("de.oth-regensburg.donkeykong", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), &game_state);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    // Clean up
    level_cleanup(&game_state.level);
    sprites_cleanup(&game_state);
    free(game_state.pressed_keys);

    return status;
}
