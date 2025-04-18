#include "game.h"
#include "player.h"
#include "physics.h"
#include "sprites.h"

void update_player(GtkWidget *drawing_area, game_state_t *game_state, float dt_seconds);

gboolean draw(GtkWidget *drawing_area, cairo_t *cr, gpointer user_data) {
    game_state_t *game_state = (game_state_t*) user_data;
    GtkAllocation allocation;
    gtk_widget_get_allocation(drawing_area, &allocation);

    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_rectangle(cr, 0, 0 , allocation.width, allocation.height);
    cairo_fill(cr);

    player_draw(cr, &game_state->player);
    return G_SOURCE_CONTINUE;
}

gboolean update(GtkWidget *drawing_area, GdkFrameClock *clock, gpointer user_data) {
    game_state_t *game_state = (game_state_t*) user_data;
    static gint64 previous_time = 0;
    gint64 current_time = gdk_frame_clock_get_frame_time(clock);
    if (previous_time == 0) {
        previous_time = current_time;
        return G_SOURCE_CONTINUE;
    }

    gint64 time_delta = current_time - previous_time;
    float dt_seconds = time_delta / 1000000.f;
    previous_time = current_time;

    update_player(drawing_area, game_state, dt_seconds);

    gtk_widget_queue_draw(drawing_area);
    return G_SOURCE_CONTINUE;
}

void update_player(GtkWidget *drawing_area, game_state_t *game_state, float dt_seconds) {
    GtkAllocation allocation;
    gtk_widget_get_allocation(drawing_area, &allocation);

    player_movement(game_state, dt_seconds, allocation.width);
    apply_physics(game_state, dt_seconds, allocation.height);

    update_player_animation_state(game_state, dt_seconds);
    update_player_animation(&game_state->player, dt_seconds);
}