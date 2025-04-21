#include "player.h"
#include "sprites.h"
#include "animation.h"

void player_init(player_t *player);
void player_draw(cairo_t *cr, player_t *player);
void player_update(GtkWidget *drawing_area, game_state_t *game_state, float dt_seconds);
void player_movement(game_state_t *game_state, float dt_seconds, float screen_width);
void player_change_animation(game_state_t *game_state, float dt_seconds);

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
    player->animation.current_animation = ANIM_IDLE;
    player->animation.current_frame_index = 0;
    player->animation.frame_time = 0;
    player->animation.frame_width = TILE_SIZE;
    player->animation.frame_height = TILE_SIZE;

    // Initialize surfaces to NULL
    player->animation.sprite_sheet = NULL;
    player->animation.current_frame = NULL;

    // Initialize Platform & Ladder with 0
    player->current_ladder_index = 0;
    player->current_platform_index = 0;
}

void player_draw(cairo_t *cr, player_t *player) {
    cairo_save(cr);

    float scale_x = SCALE * player->direction;
    float scale_y = SCALE;

    float draw_x = player->direction == -1 ? player->x + TILE_SIZE * SCALE : player->x;

    cairo_translate(cr, draw_x, player->y);
    cairo_scale(cr, scale_x, scale_y);

    cairo_pattern_t *pattern = cairo_pattern_create_for_surface(player->animation.current_frame);
    cairo_pattern_set_filter(pattern, CAIRO_FILTER_NEAREST); 

    cairo_set_source(cr, pattern);
    cairo_paint(cr);

    cairo_pattern_destroy(pattern);
    cairo_restore(cr);
}

void player_update(GtkWidget *drawing_area, game_state_t *game_state, float dt_seconds) {
    player_movement(game_state, dt_seconds, BASE_WIDTH);

    player_change_animation(game_state, dt_seconds);
    update_animation_progress(&game_state->player.animation, dt_seconds);
}

void player_movement(game_state_t *game_state, float dt_seconds, float screen_width) {
    float move_amount = MOVE_SPEED * dt_seconds;
    bool key_left = game_state->pressed_keys['a'] || game_state->pressed_keys['A'];
    bool key_right = game_state->pressed_keys['d'] || game_state->pressed_keys['D'];
    bool key_up = game_state->pressed_keys['w'] || game_state->pressed_keys['W'];
    bool key_down = game_state->pressed_keys['s'] || game_state->pressed_keys['S'];
    
    float player_bottom = game_state->player.y + PLAYER_HEIGHT;
    int current_ladder = game_state->player.current_ladder_index;
    structure_t *ladder = &game_state->level.ladders[current_ladder];

    if (key_left) {
        game_state->player.x -= move_amount;
        game_state->player.direction = -1;
    }
    if (key_right) {
        game_state->player.x += move_amount;
        game_state->player.direction = 1;
    }
    if (key_up && game_state->player.on_ladder) {
        float ladder_top = ladder->y - LADDER_EXTRA;
        if ((player_bottom - 2) >= ladder_top) {
            game_state->player.y -= move_amount;
        }
    }
    if (key_down && game_state->player.on_ladder) {
        float ladder_bottom = ladder->y + ladder->height;
        if (player_bottom <= ladder_bottom - 2) {
            game_state->player.y += move_amount;
        }
    }
    if (game_state->pressed_keys[GDK_KEY_space] && game_state->player.is_grounded) {
        game_state->player.velocity_y = -JUMP_FORCE;
        game_state->player.is_grounded = false;
    }
}

// determines and sets the appropriate animation state for the player 
void player_change_animation(game_state_t *game_state, float dt_seconds) {
    player_t *player = &game_state->player;
    bool key_left = game_state->pressed_keys['a'] || game_state->pressed_keys['A'];
    bool key_right = game_state->pressed_keys['d'] || game_state->pressed_keys['D'];
    bool key_up = game_state->pressed_keys['w'] || game_state->pressed_keys['W'];
    bool key_down = game_state->pressed_keys['s'] || game_state->pressed_keys['S'];

    float player_bottom = game_state->player.y + PLAYER_HEIGHT;
    int current_ladder = game_state->player.current_ladder_index;
    structure_t *ladder = &game_state->level.ladders[current_ladder];
    int current_platform = game_state->player.current_platform_index;
    structure_t *platform = &game_state->level.platforms[current_platform];
    float ladder_top = ladder->y - platform->height;
    float ladder_bottom = ladder->y + ladder->height - 2;

    if (player->on_ladder && (key_down || key_up)) {
        player->climbing = true;
    }
    else {
        player->climbing = false;
    }

    if (!player->is_grounded && !player->on_ladder) {
        set_animation(&player->animation, ANIM_JUMP);
    }
    else if (player->on_ladder && key_up && player_bottom > ladder_top) {
        set_animation(&player->animation, ANIM_CLIMB);
    }
    else if (player->on_ladder && key_down && player_bottom < ladder_bottom) {
        set_animation(&player->animation, ANIM_CLIMB);
    }
    else if (player->on_ladder && player->climbing && player_bottom > ladder_top) {
        set_animation(&player->animation, ANIM_CLIMB_IDLE);
    }
    else if (player->is_grounded && (key_left || key_right)) {
        set_animation(&player->animation, ANIM_WALK);
    }
    else {
        set_animation(&player->animation, ANIM_IDLE);
    }
}