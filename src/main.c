#include <gtk/gtk.h>
#include <glib.h>
#include <assert.h>
#include <time.h>
#include "types.h"
#include "consts.h"
#include "core/game.h"
#include "core/cmdline.h"
#include "core/input.h"
#include "level/level.h"
#include "core/sprite/sprite.h"
#include "core/sprite/sprite_utils.h"
#include "level/loader.h"
#include "core/gui/gui.h"

static gboolean maximize_window(gpointer data);
static void activate(GtkApplication* app, gpointer user_data);
int main(int argc, char **argv);

static gboolean maximize_window(gpointer data) {
    GtkWindow *window = GTK_WINDOW(data);
    gtk_window_maximize(window);
    return G_SOURCE_REMOVE; 
}

// Setup Gdk Window
static void activate(GtkApplication* app, gpointer user_data) {
    game_state_t *game_state = (game_state_t*) user_data;
    arguments_t *arguments = &game_state->arguments;

    // Set window properties
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Donkey Kong");
    gtk_window_set_default_size(GTK_WINDOW(window), arguments->width, arguments->height);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    // Add Drawing Area
    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(window), drawing_area);
    gtk_widget_show_all(window);

    if (arguments->fullscreen) {
        // maximize window after 10ms
        g_timeout_add(10, maximize_window, window);
    }

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

    // Default argument values
    game_state.arguments.width = BASE_WIDTH;
    game_state.arguments.height = BASE_HEIGHT;
    game_state.arguments.fullscreen = FALSE;
    game_state.arguments.start_level = 0;
    game_state.arguments.level_folder_path = DATA_FOLDER_PATH;

    // Parse arguments
    if (parse_cmdline(argc, argv, &game_state.arguments)) {
        g_warning("Error handling command line arguments");
        return 1; 
    }

    // Init game and input
    game_init(&game_state);
    input_init(&game_state);
    gui_init(&game_state);

    int file_path_len = strlen(game_state.arguments.level_folder_path) + strlen(ASSETS_FILE_PATH) + 1;
    char *assets_file_path = malloc(file_path_len);
    if (assets_file_path) {
        snprintf(assets_file_path, file_path_len, "%s%s", game_state.arguments.level_folder_path, ASSETS_FILE_PATH);
    } else {
        g_warning("Failed to allocate memory for assets path");
    }

    if (sprite_load_from_json(&game_state, assets_file_path)) {
        if (game_state.arguments.start_level != 0 && game_state.arguments.start_level <= MAX_LEVELS) {
            game_state.current_level = game_state.arguments.start_level;
            level_load(&game_state, LEVEL_FILE_PATH, game_state.current_level);
            game_state.mode = GAME_MODE_NORMAL;
        } else {
            level_load(&game_state, MENU_FILE_PATH, 0);
        }

        // Init Gdk Window
        app = gtk_application_new("de.oth-regensburg.donkeykong", G_APPLICATION_DEFAULT_FLAGS);
        g_signal_connect(app, "activate", G_CALLBACK(activate), &game_state);
        status = g_application_run(G_APPLICATION(app), 0, NULL);

        // Clean up
        g_object_unref(app);
        level_cleanup(&game_state.level);
    }

    free(assets_file_path);
    input_cleanup(&game_state);
    sprite_cleanup();
    g_hash_table_destroy(get_string_to_enum_table());

    return status;
}
