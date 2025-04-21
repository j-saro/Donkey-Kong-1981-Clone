#include "player.h"
#include "animation.h"

void player_init(game_state_t *game_state);
void player_load_sprites(game_state_t *game_state);
void player_cleanup(player_t *player);
void player_draw(cairo_t *cr, player_t *player);
void player_update(GtkWidget *drawing_area, game_state_t *game_state, float dt_seconds);
void player_movement(game_state_t *game_state, float dt_seconds, float screen_width);
void player_change_animation(game_state_t *game_state, float dt_seconds);

void player_init(game_state_t *game_state) {
    player_t *player = &game_state->player;

    // Player Position
    player->base.x = 120;
    player->base.y = 530;
    player->base.velocity_x = 0;
    player->base.velocity_y = 0;
    player->base.direction = 1;
    player->base.is_grounded = false;
    player->climbing = false;
    player->on_ladder = false;

    // Player Animation
    player->base.animation.current_animation = ANIM_IDLE;
    player->base.animation.current_frame_index = 0;
    player->base.animation.frame_time = 0;
    player->base.animation.frame_width = TILE_SIZE;
    player->base.animation.frame_height = TILE_SIZE;

    // Initialize surfaces to NULL
    player->base.animation.sprite_sheet = NULL;
    player->base.animation.current_frame = NULL;

    // Initialize Platform & Ladder with 0
    player->current_ladder_index = 0;
    player->current_platform_index = 0;

    player_load_sprites(game_state);
}

void player_load_sprites(game_state_t *game_state) {
    // Load player sprite
    game_state->player.base.animation.sprite_sheet = cairo_image_surface_create_from_png("./assets/mario_sprite_sheet.png");
    if (cairo_surface_status(game_state->player.base.animation.sprite_sheet) != CAIRO_STATUS_SUCCESS) {
        g_warning("Failed to create player spritesheet");
        return;
    }
    
    // Load first animation
    player_t *player = &game_state->player;
    update_animation_frame(&player->base.animation);
    if (game_state->player.base.animation.current_frame == NULL || 
        cairo_surface_status(game_state->player.base.animation.current_frame) != CAIRO_STATUS_SUCCESS) {
        g_warning("Failed to create initial frame surface");
    }
}

void player_cleanup(player_t *player) {
    // Cleanup Current Animation Sprite
    if (player->base.animation.current_frame != NULL) {
        cairo_surface_destroy(player->base.animation.current_frame);
        player->base.animation.current_frame = NULL;
    }
        
    // Cleanup Player spritesheet
    if (player->base.animation.sprite_sheet != NULL) {
        cairo_surface_destroy(player->base.animation.sprite_sheet);
        player->base.animation.sprite_sheet = NULL;
    }
}

void player_draw(cairo_t *cr, player_t *player) {
    cairo_save(cr);

    float scale_x = SCALE * player->base.direction;
    float scale_y = SCALE;

    float draw_x = player->base.direction == -1 ? player->base.x + TILE_SIZE * SCALE : player->base.x;

    cairo_translate(cr, draw_x, player->base.y);
    cairo_scale(cr, scale_x, scale_y);

    cairo_pattern_t *pattern = cairo_pattern_create_for_surface(player->base.animation.current_frame);
    cairo_pattern_set_filter(pattern, CAIRO_FILTER_NEAREST); 

    cairo_set_source(cr, pattern);
    cairo_paint(cr);

    cairo_pattern_destroy(pattern);
    cairo_restore(cr);
}

void player_update(GtkWidget *drawing_area, game_state_t *game_state, float dt_seconds) {
    player_movement(game_state, dt_seconds, BASE_WIDTH);

    player_change_animation(game_state, dt_seconds);
    update_animation_progress(&game_state->player.base.animation, dt_seconds);
}

void player_movement(game_state_t *game_state, float dt_seconds, float screen_width) {
    float move_amount = MOVE_SPEED * dt_seconds;
    bool key_left = game_state->pressed_keys['a'] || game_state->pressed_keys['A'];
    bool key_right = game_state->pressed_keys['d'] || game_state->pressed_keys['D'];
    bool key_up = game_state->pressed_keys['w'] || game_state->pressed_keys['W'];
    bool key_down = game_state->pressed_keys['s'] || game_state->pressed_keys['S'];
    
    float player_bottom = game_state->player.base.y + PLAYER_HEIGHT;
    int current_ladder = game_state->player.current_ladder_index;
    structure_t *ladder = &game_state->level.ladders[current_ladder];

    if (key_left) {
        game_state->player.base.x -= move_amount;
        game_state->player.base.direction = -1;
    }
    if (key_right) {
        game_state->player.base.x += move_amount;
        game_state->player.base.direction = 1;
    }
    if (key_up && game_state->player.on_ladder) {
        float ladder_top = ladder->y - LADDER_EXTRA;
        if ((player_bottom - 2) >= ladder_top) {
            game_state->player.base.y -= move_amount;
        }
    }
    if (key_down && game_state->player.on_ladder) {
        float ladder_bottom = ladder->y + ladder->height;
        if (player_bottom <= ladder_bottom - 2) {
            game_state->player.base.y += move_amount;
        }
    }
    if (game_state->pressed_keys[GDK_KEY_space] && game_state->player.base.is_grounded) {
        game_state->player.base.velocity_y = -JUMP_FORCE;
        game_state->player.base.is_grounded = false;
    }
}

// determines and sets the appropriate animation state for the player 
void player_change_animation(game_state_t *game_state, float dt_seconds) {
    player_t *player = &game_state->player;
    bool key_left = game_state->pressed_keys['a'] || game_state->pressed_keys['A'];
    bool key_right = game_state->pressed_keys['d'] || game_state->pressed_keys['D'];
    bool key_up = game_state->pressed_keys['w'] || game_state->pressed_keys['W'];
    bool key_down = game_state->pressed_keys['s'] || game_state->pressed_keys['S'];

    float player_bottom = game_state->player.base.y + PLAYER_HEIGHT;
    int current_ladder = game_state->player.current_ladder_index;
    structure_t *ladder = &game_state->level.ladders[current_ladder];
    int current_platform = game_state->player.current_platform_index;
    structure_t *platform = &game_state->level.platforms[current_platform];
    float ladder_top = ladder->y - platform->height;
    float ladder_bottom = ladder->y + ladder->height - 2;

    if (!player->base.is_grounded && !player->on_ladder) {
        player->climbing = false;
        set_animation(&player->base.animation, ANIM_JUMP);
    }
    else if (player->on_ladder && key_up && player_bottom > ladder_top) {
        player->climbing = true;
        set_animation(&player->base.animation, ANIM_CLIMB);
    }
    else if (player->on_ladder && key_down && player_bottom < ladder_bottom) {
        player->climbing = true;
        set_animation(&player->base.animation, ANIM_CLIMB);
    }
    else if (player->on_ladder && player->climbing && player_bottom > ladder_top) {
        player->climbing = true;
        set_animation(&player->base.animation, ANIM_CLIMB_IDLE);
    }
    else if (player->base.is_grounded && (key_left || key_right)) {
        player->climbing = false;
        set_animation(&player->base.animation, ANIM_WALK);
    }
    else {
        player->climbing = false;
        set_animation(&player->base.animation, ANIM_IDLE);
    }
}