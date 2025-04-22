#include "player.h"
#include "animation.h"
#include "movable_entity.h"

void player_init(player_t *player);
void player_load_sprites(movable_entity_t *base);
void player_cleanup(movable_entity_t *base);
void player_draw(cairo_t *cr, movable_entity_t *base);
void player_update(GtkWidget *drawing_area, game_state_t *game_state, float dt_seconds);
void player_movement(game_state_t *game_state, float dt_seconds, float screen_width);
void player_change_animation(game_state_t *game_state, float dt_seconds);

void player_init(player_t *player) {
    movable_entity_t *base = &player->base;
    // Player Position
    base->x = 120;
    base->y = 530;
    base->velocity_x = 0;
    base->velocity_y = 0;
    base->direction = 1;
    base->is_grounded = false;
    player->climbing = false;
    player->on_ladder = false;

    // Player Animation
    base->animation.current_animation = ANIM_IDLE_MARIO;
    base->animation.current_frame_index = 0;
    base->animation.frame_time = 0;
    base->animation.frame_width = TILE_SIZE;
    base->animation.frame_height = TILE_SIZE;

    // Initialize surfaces to NULL
    base->animation.sprite_sheet = NULL;
    base->animation.current_frame = NULL;

    // Initialize Platform & Ladder with 0
    player->current_ladder_index = 0;
    player->current_platform_index = 0;

    player_load_sprites(&player->base);
}

void player_load_sprites(movable_entity_t *base) {
    const char *spritesheet = "./assets/mario_sprite_sheet.png";
    movable_entitiy_load_sprites(base, spritesheet);
}

void player_cleanup(movable_entity_t *base) {
    movable_entitiy_cleanup(base);
}

void player_draw(cairo_t *cr, movable_entity_t *base) {
    movable_entitiy_draw(cr, base);
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
        set_animation(&player->base.animation, ANIM_JUMP_MARIO);
    }
    else if (player->on_ladder && key_up && player_bottom > ladder_top) {
        player->climbing = true;
        set_animation(&player->base.animation, ANIM_CLIMB_MARIO);
    }
    else if (player->on_ladder && key_down && player_bottom < ladder_bottom) {
        player->climbing = true;
        set_animation(&player->base.animation, ANIM_CLIMB_MARIO);
    }
    else if (player->on_ladder && player->climbing && player_bottom > ladder_top) {
        player->climbing = true;
        set_animation(&player->base.animation, ANIM_CLIMB_IDLE_MARIO);
    }
    else if (player->base.is_grounded && (key_left || key_right)) {
        player->climbing = false;
        set_animation(&player->base.animation, ANIM_WALK_MARIO);
    }
    else {
        player->climbing = false;
        set_animation(&player->base.animation, ANIM_IDLE_MARIO);
    }
}