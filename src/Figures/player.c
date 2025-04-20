#include "player.h"
#include "sprites.h"
#include "animation.h"

void player_init(player_t *player);
void player_draw(cairo_t *cr, player_t *player);
void player_update(GtkWidget *drawing_area, game_state_t *game_state, float dt_seconds);
void player_movement(game_state_t *game_state, float dt_seconds, float screen_width);

void player_init(player_t *player) {
    // Player Position
    player->x = 120;
    player->y = 530;
    player->velocity_x = 0;
    player->velocity_y = 0;
    player->direction = 1;
    player->is_grounded = false;
    player->climbing = false;
    player->on_ladder = false;

    // Player Animation
    player->current_animation = ANIM_IDLE;
    player->current_frame_index = 0;
    player->frame_time = 0;
    player->frame_width = TILE_SIZE;
    player->frame_height = TILE_SIZE;

    // Initialize surfaces to NULL
    player->sprite_sheet = NULL;
    player->current_frame = NULL;
}

void player_draw(cairo_t *cr, player_t *player) {
    cairo_save(cr);

    float scale_x = SCALE * player->direction;
    float scale_y = SCALE;

    float draw_x = player->direction == -1 ? player->x + TILE_SIZE * SCALE : player->x;

    cairo_translate(cr, draw_x, player->y);
    cairo_scale(cr, scale_x, scale_y);

    cairo_pattern_t *pattern = cairo_pattern_create_for_surface(player->current_frame);
    cairo_pattern_set_filter(pattern, CAIRO_FILTER_NEAREST); 

    cairo_set_source(cr, pattern);
    cairo_paint(cr);

    cairo_pattern_destroy(pattern);
    cairo_restore(cr);
}

void player_update(GtkWidget *drawing_area, game_state_t *game_state, float dt_seconds) {
    player_movement(game_state, dt_seconds, BASE_WIDTH);

    update_player_animation_state(game_state, dt_seconds);
    update_player_animation(&game_state->player, dt_seconds);
}

void player_movement(game_state_t *game_state, float dt_seconds, float screen_width) {
    float move_amount = MOVE_SPEED * dt_seconds;
    
    if (game_state->pressed_keys['a'] || game_state->pressed_keys['A']) {
        game_state->player.x -= move_amount;
        game_state->player.direction = -1;
    }
    if (game_state->pressed_keys['d'] || game_state->pressed_keys['D']) {
        game_state->player.x += move_amount;
        game_state->player.direction = 1;
    }
    if ((game_state->pressed_keys['w'] || game_state->pressed_keys['W']) &&
         game_state->player.on_ladder) {
        
        game_state->player.y -= move_amount;
    }
    if ((game_state->pressed_keys['s'] || game_state->pressed_keys['S']) &&
        game_state->player.on_ladder) {

        float player_bottom = game_state->player.y + PLAYER_HEIGHT;
        int current_ladder = game_state->player.current_ladder_index;
        structure_t *ladder = &game_state->level.ladders[current_ladder];
        float ladder_bottom = ladder->position.y + ladder->height;

        if (player_bottom <= ladder_bottom - 2) {
            game_state->player.y += move_amount;
        }
    }
    if (game_state->pressed_keys[GDK_KEY_space] && game_state->player.is_grounded) {
        game_state->player.velocity_y = -JUMP_FORCE;
        game_state->player.is_grounded = false;
    }
}
