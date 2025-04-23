#include <gtk/gtk.h>
#include <assert.h>
#include <time.h>
#include "types.h"
#include "core/game.h"
#include "core/input.h"
#include "level/level.h"

static void activate(GtkApplication* app, gpointer user_data);

static void activate(GtkApplication* app, gpointer user_data) {
    game_state_t *game_state = (game_state_t*) user_data;

    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Donkey Kong 1981");
    gtk_window_set_default_size(GTK_WINDOW(window), BASE_WIDTH, BASE_HEIGHT);

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
    srand((unsigned)time(NULL)); // seed for rand function

    // init
    input_init(&game_state);
    level_init(&game_state);

    app = gtk_application_new("de.oth-regensburg.donkeykong", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), &game_state);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    // Clean up
    input_cleanup(&game_state);
    level_cleanup(&game_state.level);

    return status;
}
