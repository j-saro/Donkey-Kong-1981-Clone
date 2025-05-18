#include <gtk/gtk.h>
#include <assert.h>
#include <time.h>
#include "types.h"
#include "consts.h"
#include "core/game.h"
#include "core/input.h"
#include "level/level.h"
#include "core/sprite/sprite.h"
#include "core/sprite/sprite_utils.h"
#include "level/loader.h"
#include "core/gui.h"

int main(int argc, char **argv);
static void activate(GtkApplication* app, gpointer user_data);

// Setup Gdk Window
static void activate(GtkApplication* app, gpointer user_data) {
    game_state_t *game_state = (game_state_t*) user_data;

    // Set window properties
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Donkey Kong");
    gtk_window_set_default_size(GTK_WINDOW(window), BASE_WIDTH, BASE_HEIGHT);

    // Add Drawing Area
    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(window), drawing_area);
    gtk_widget_show_all(window);

    // Connect Input handlers and draw event
    g_signal_connect(window, "key_press_event", G_CALLBACK(on_key_pressed), game_state);
    g_signal_connect(window, "key_release_event", G_CALLBACK(on_key_released), game_state);
    g_signal_connect(drawing_area, "draw", G_CALLBACK(draw), game_state);

    // Update game state every tick
    gtk_widget_add_tick_callback(drawing_area, update, game_state, NULL);
}

int main(int argc, char **argv) {
    srand((unsigned)time(NULL)); // seed for rand function

    GtkApplication *app;
    int status;
    game_state_t game_state;

    // Init game and input
    game_init(&game_state);
    input_init(&game_state);
    gui_init(&game_state);

    if (sprite_load_from_json(ASSETS_FILE_PATH)) {
        level_load(&game_state);

        // Init Gdk Window
        app = gtk_application_new("de.oth-regensburg.donkeykong", G_APPLICATION_DEFAULT_FLAGS);
        g_signal_connect(app, "activate", G_CALLBACK(activate), &game_state);
        status = g_application_run(G_APPLICATION(app), argc, argv);

        // Clean up
        g_object_unref(app);
        level_cleanup(&game_state.level);
    }

    input_cleanup(&game_state);
    sprite_cleanup();
    g_hash_table_destroy(get_string_to_enum_table());

    return status;
}
