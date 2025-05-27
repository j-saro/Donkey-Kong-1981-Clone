#include <math.h>
#include "consts.h"
#include "core/game.h"
#include "level/level.h"
#include "level/cutscene.h"
#include "entities/abstract/effect.h"
#include "entities/abstract/enemy.h"
#include "entities/characters/player.h"
#include "core/physics/physics.h"
#include "core/physics/physics_geometry.h"
#include "core/input.h"
#include "core/gui.h"

void game_init(game_state_t *game_state);
gboolean draw(GtkWidget *drawing_area, cairo_t *cr, gpointer user_data);
gboolean update(GtkWidget *drawing_area, GdkFrameClock *clock, gpointer user_data);
void game_update(game_state_t *game_state, float dt_seconds);

void game_init(game_state_t *game_state) {
    game_state->mode = GAME_MODE_MENU;
    game_state->key_cooldown = KEY_INPUT_COOLDOWN;

    level_init(game_state);
}

gboolean draw(GtkWidget *drawing_area, cairo_t *cr, gpointer user_data) {
    game_state_t *game_state = (game_state_t*) user_data;
    GtkAllocation alloc;
    gtk_widget_get_allocation(drawing_area, &alloc);

    // calc scale
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

    // clear entire window
    cairo_set_source_rgb(cr, 0, 0, 0); // black background for the game content area
    cairo_rectangle(cr, 0, 0, window_width, window_height);
    cairo_fill(cr);

    // center + scale the scene for game drawing
    cairo_save(cr);
    cairo_translate(cr, offset_x, offset_y);
    cairo_scale(cr, scale, scale);

    level_draw(cr, game_state);
    
    // GUIs
    if (game_state->mode == GAME_MODE_MENU) {
        // Start menu
        gui_startmenu_draw(cr, game_state);
    }
    else if (game_state->mode == GAME_MODE_GAME_FINISH) {
        // Finish screen text
        gui_game_complete_draw(cr, game_state);
    }
    else if ( game_state->mode == GAME_MODE_GAME_OVER) {
        // Finish screen text
        gui_game_over_draw(cr, game_state);
    }

    if (game_state->mode != GAME_MODE_MENU && game_state->mode != GAME_MODE_CUTSCENE) {
        // Game gui
        gui_game_draw(cr, game_state);
    }

    cairo_restore(cr);

    // draw grey offset areas
    cairo_set_source_rgb(cr, 0.2, 0.2, 0.2); // Grey color

    // Top border
    cairo_rectangle(cr, 0, 0, window_width, offset_y);
    cairo_fill(cr);

    // Bottom border
    cairo_rectangle(cr, 0, window_height - offset_y, window_width, offset_y);
    cairo_fill(cr);

    // Left border
    cairo_rectangle(cr, 0, offset_y, offset_x, window_height - (2 * offset_y));
    cairo_fill(cr);

    // Right border
    cairo_rectangle(cr, window_width - offset_x, offset_y, offset_x, window_height - (2 * offset_y));
    cairo_fill(cr);

    return G_SOURCE_CONTINUE;
}

gboolean update(GtkWidget *drawing_area, GdkFrameClock *clock, gpointer user_data) {
    game_state_t *game_state = (game_state_t*) user_data;

    // Calc delta time
    static gint64 previous_time = 0;
    gint64 current_time = gdk_frame_clock_get_frame_time(clock);
    if (previous_time == 0) {
        previous_time = current_time;
        return G_SOURCE_CONTINUE;
    }

    gint64 time_delta = current_time - previous_time;
    float dt_seconds = time_delta / 1000000.f;
    previous_time = current_time;

    // update key cooldown
    game_state->key_cooldown -= dt_seconds;

    // update entire game loop
    game_update(game_state, dt_seconds);
    
    gtk_widget_queue_draw(drawing_area);
    return G_SOURCE_CONTINUE;
}

void game_update(game_state_t *game_state, float dt_seconds) {
    // Pressed keys
    bool key_skip = is_key_pressed(game_state, GDK_KEY_c) || is_key_pressed(game_state, GDK_KEY_C);
    bool key_enter = is_key_pressed(game_state, GDK_KEY_Return) || is_key_pressed(game_state, GDK_KEY_KP_Enter);
    bool key_space = is_key_pressed(game_state, GDK_KEY_space);

    if (game_state->key_cooldown <= 0) {
        // start game
        if (key_enter && game_state->mode == GAME_MODE_MENU) {
            game_state->mode = GAME_MODE_CUTSCENE;
            // Reset game
            level_init(game_state);
            level_cleanup(&game_state->level);
            level_load(game_state, CUTSCENE_FILE_PATH, CUTSCENE_DK_INTRO);
            game_state->key_cooldown = KEY_INPUT_COOLDOWN;
        }

        // back to menu
        if (key_space && 
            (game_state->mode == GAME_MODE_GAME_FINISH || 
            game_state->mode == GAME_MODE_GAME_OVER)) {
            game_state->mode = GAME_MODE_MENU;
            // Reset game
            level_init(game_state);
            level_cleanup(&game_state->level);
            level_load(game_state, MENU_FILE_PATH, 0);
            game_state->key_cooldown = KEY_INPUT_COOLDOWN;
        }
        
        // skip current cutscene
        if (key_skip) {
            if (game_state->mode == GAME_MODE_CUTSCENE) {
                effect_clear_all(&game_state->level);
                switch (game_state->current_cutscene) {
                    case CUTSCENE_DK_INTRO:
                    case CUTSCENE_DK_ESCAPE:
                        game_state->cutscene_step = 4;
                        break;
                    case CUTSCENE_DK_DEATH:
                        game_state->cutscene_step = 3;
                        break;
                    case CUTSCENE_MARIO_DEATH:
                        game_state->cutscene_step = 1;
                        break;
                    default:
                        break;
                }
            }
            game_state->key_cooldown = KEY_INPUT_COOLDOWN;
        }
    }

    // Handle different game modes
    switch (game_state->mode) {
        case GAME_MODE_MENU:
            game_state->level.player.previous_y = game_state->level.player.base.y;
            player_update(game_state, dt_seconds);
            apply_physics(game_state, dt_seconds, BASE_HEIGHT);
            break;

        // Normal game loop
        case GAME_MODE_NORMAL:
            donkey_kong_t *donkey_kong = &game_state->level.donkey_kong;
            game_state->level.player.previous_y = game_state->level.player.base.y;
            level_update(game_state, dt_seconds);
            break;
    
        // current cutscene
        case GAME_MODE_CUTSCENE:
            cutscene_load(game_state, dt_seconds);
            break;

        // only effect
        case GAME_MODE_EFFECT:
            effect_update(&game_state->level, dt_seconds);
            if (!effect_is_active(&game_state->level, ANIM_ENEMY_DEATH)) {
                game_state->mode = GAME_MODE_NORMAL;
            }
            break;

        default:
            // Do not update game
            break;
    }
}