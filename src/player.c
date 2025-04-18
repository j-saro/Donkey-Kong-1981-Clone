#include "player.h"

void player_init(player_t *player) {
    player->x = 0;
    player->y = 0;
    player->velocity_x = 0;
    player->velocity_y = 0;
    player->is_grounded = false;
}

void player_draw(cairo_t *cr, const player_t *player) {
    cairo_save(cr);

    float scale_x = SCALE;
    float scale_y = SCALE;

    cairo_translate(cr, player->x, player->y);
    cairo_scale(cr, scale_x, scale_y);

    cairo_pattern_t *pattern = cairo_pattern_create_for_surface(player->sprite);
    cairo_pattern_set_filter(pattern, CAIRO_FILTER_NEAREST); 

    cairo_set_source(cr, pattern);
    cairo_paint(cr);

    cairo_pattern_destroy(pattern);
    cairo_restore(cr);
}

void player_movement(game_state_t *game_state, float dt_seconds, float screen_width) {
    float move_amount = MOVE_SPEED * dt_seconds;
    
    if (game_state->pressed_keys['a'] || game_state->pressed_keys['A']) {
        if (game_state->player.x > 0) {
            game_state->player.x -= move_amount;
        }
    }
    if (game_state->pressed_keys['d'] || game_state->pressed_keys['D']) {
        if (game_state->player.x < screen_width - PLAYER_WIDTH) {
            game_state->player.x += move_amount;
        }
    }
    
    if (game_state->pressed_keys[GDK_KEY_space] && game_state->player.is_grounded) {
        game_state->player.velocity_y = -JUMP_FORCE;
        game_state->player.is_grounded = false;
    }
}