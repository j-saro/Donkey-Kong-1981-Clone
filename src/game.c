#include "game.h"
#include "player.h"
#include "level.h"
#include <math.h>


gboolean draw(GtkWidget *drawing_area, cairo_t *cr, gpointer user_data);
gboolean update(GtkWidget *drawing_area, GdkFrameClock *clock, gpointer user_data);

gboolean draw(GtkWidget *drawing_area, cairo_t *cr, gpointer user_data) {
    game_state_t *game_state = (game_state_t*) user_data;
    GtkAllocation alloc;
    gtk_widget_get_allocation(drawing_area, &alloc);

    float window_width = alloc.width;
    float window_height = alloc.height;
    float scale_x = window_width / (float)BASE_WIDTH;
    float scale_y = window_height / (float)BASE_HEIGHT;
    float scale = fmin(scale_x, scale_y);

    // calc how much empty space on each side
    float scene_w = BASE_WIDTH  * scale;
    float scene_h = BASE_HEIGHT * scale;
    float offset_x = (window_width - scene_w) / 2.0f;
    float offset_y = (window_height - scene_h) / 2.0f;

    // clear the window
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_rectangle(cr, 0, 0, window_width, window_height);
    cairo_fill(cr);

    // center + scale the scene
    cairo_save(cr);
    cairo_translate(cr, offset_x, offset_y);
    cairo_scale(cr, scale, scale);

    // draw in logical coords (0..BASE_WIDTH, 0..BASE_HEIGHT)
    level_draw(cr, &game_state->level);
    player_draw(cr, &game_state->player);
    cairo_restore(cr);

    // draw border or HUD in real window‐space
    cairo_set_source_rgb(cr, 1, 0, 0);
    cairo_set_line_width(cr, 4.0);
    cairo_rectangle(cr, 0, 0, window_width, window_height);
    cairo_stroke(cr);

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

    player_update(drawing_area, game_state, dt_seconds);

    gtk_widget_queue_draw(drawing_area);
    return G_SOURCE_CONTINUE;
}
