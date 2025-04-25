#include "entities/characters/player.h"
#include "core/animation.h"
#include "entities/abstract/movable_entity.h"

void player_init(player_t *player, cJSON *json);
void player_draw(cairo_t *cr, const movable_entity_t *base);
void player_update(GtkWidget *drawing_area, game_state_t *game_state, float dt_seconds);
void player_movement(game_state_t *game_state, float dt_seconds, float screen_width);
void player_change_animation(game_state_t *game_state, float dt_seconds);

void player_init(player_t *player, cJSON *json) {
    player->base.type = MARIO;
    
    // Json read Values
    movable_entity_parse(&player->base, json);

    // Player Default Values
    // Animation
    player->base.animation.current_frame = NULL;
    
    // Position
    player->climbing = false;
    player->on_ladder = false;
    player->current_ladder_index = 0;
    player->current_platform_index = 0;
}


void player_draw(cairo_t *cr, const movable_entity_t *base) {
    movable_entity_draw(cr, base);
}

void player_update(GtkWidget *drawing_area, game_state_t *game_state, float dt_seconds) {
    player_movement(game_state, dt_seconds, BASE_WIDTH);

    player_change_animation(game_state, dt_seconds);
    update_animation_progress(&game_state->level.player.base, dt_seconds);
}

void player_movement(game_state_t *game_state, float dt_seconds, float screen_width) {
    player_t *player = &game_state->level.player;

    float move_amount = MOVE_SPEED * dt_seconds;

    bool key_left = game_state->pressed_keys['a'] || game_state->pressed_keys['A'];
    bool key_right = game_state->pressed_keys['d'] || game_state->pressed_keys['D'];
    bool key_up = game_state->pressed_keys['w'] || game_state->pressed_keys['W'];
    bool key_down = game_state->pressed_keys['s'] || game_state->pressed_keys['S'];
    
    float player_bottom = player->base.y + PLAYER_HEIGHT;
    int current_ladder = player->current_ladder_index;
    geometry_t *ladder = &game_state->level.ladders[current_ladder];

    if (key_left) {
        player->base.x -= move_amount;
        player->base.direction = -1;
    }
    if (key_right) {
        player->base.x += move_amount;
        player->base.direction = 1;
    }
    if (key_up && player->on_ladder) {
        float ladder_top = ladder->y - PHYSICS_EPSILON;
        if ((player_bottom - 2) >= ladder_top) {
            player->base.y -= move_amount;
        }
    }
    if (key_down && player->on_ladder) {
        float ladder_bottom = ladder->y + ladder->height;
        if (player_bottom <= ladder_bottom - 2) {
            player->base.y += move_amount;
        }
    }
    if (game_state->pressed_keys[GDK_KEY_space] && player->base.is_grounded) {
        player->base.velocity_y = -JUMP_FORCE;
        player->base.is_grounded = false;
    }
}

// determines and sets the appropriate animation state for the player 
void player_change_animation(game_state_t *game_state, float dt_seconds) {
    player_t *player = &game_state->level.player;

    bool key_left = game_state->pressed_keys['a'] || game_state->pressed_keys['A'];
    bool key_right = game_state->pressed_keys['d'] || game_state->pressed_keys['D'];
    bool key_up = game_state->pressed_keys['w'] || game_state->pressed_keys['W'];
    bool key_down = game_state->pressed_keys['s'] || game_state->pressed_keys['S'];

    float player_bottom = player->base.y + PLAYER_HEIGHT;

    int current_ladder = player->current_ladder_index;
    geometry_t *ladder = &game_state->level.ladders[current_ladder];

    int current_platform = player->current_platform_index;
    geometry_t *platform = &game_state->level.platforms[current_platform];
    
    float ladder_top = ladder->y - platform->height;
    float ladder_bottom = ladder->y + ladder->height - 2;

    if (!player->base.is_grounded && !player->on_ladder) {
        player->climbing = false;
        set_animation(&player->base, ANIM_JUMP_MARIO);
    }
    else if (player->on_ladder && key_up && player_bottom > ladder_top) {
        player->climbing = true;
        set_animation(&player->base, ANIM_CLIMB_MARIO);
    }
    else if (player->on_ladder && key_down && player_bottom < ladder_bottom) {
        player->climbing = true;
        set_animation(&player->base, ANIM_CLIMB_MARIO);
    }
    else if (player->on_ladder && player->climbing && player_bottom > ladder_top) {
        player->climbing = true;
        set_animation(&player->base, ANIM_CLIMB_IDLE_MARIO);
    }
    else if (player->base.is_grounded && (key_left || key_right)) {
        player->climbing = false;
        set_animation(&player->base, ANIM_WALK_MARIO);
    }
    else {
        player->climbing = false;
        set_animation(&player->base, ANIM_IDLE_MARIO);
    }
}