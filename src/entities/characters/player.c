#include "consts.h"
#include "core/input.h"
#include "entities/characters/player.h"
#include "core/sprite/animation.h"
#include "entities/abstract/entity.h"
#include "core/physics/physics_utils.h"

void player_init(player_t *player, cJSON *json);
void player_draw(cairo_t *cr, const entity_t *base);
void player_update(game_state_t *game_state, float dt_seconds);
void player_check_jump(player_t *player, float dt_seconds);
void player_check_death(game_state_t *game_state);
void player_hammer_update(player_t *player, float dt_seconds);
void player_movement(game_state_t *game_state, float dt_seconds, float screen_width);
void player_change_animation(game_state_t *game_state, float dt_seconds);

void player_init(player_t *player, cJSON *json) {
    player->base.type = MARIO;
    
    // Json read Values
    entity_parse(&player->base, json);
    
    player->spawn_x = player->base.x;
    player->spawn_y = player->base.y;

    // Player Default Values
    player->climbing = false;
    player->on_ladder = false;
    player->current_ladder_index = 0;
    player->current_platform_index = 0;

    player->on_elevator = false;
    player->flight_timer = 0.0f;
    player->hammer_time = 0.0f;
    player->has_hammer = false;
    player->is_dead = false;
}


void player_draw(cairo_t *cr, const entity_t *base) {
    entity_draw(cr, base);
}

void player_update(game_state_t *game_state, float dt_seconds) {
    player_check_jump(&game_state->level.player, dt_seconds);
    player_hammer_update(&game_state->level.player, dt_seconds);
    player_movement(game_state, dt_seconds, BASE_WIDTH);

    player_change_animation(game_state, dt_seconds);
    update_animation_progress(&game_state->level.player.base, dt_seconds);
}

void player_check_jump(player_t *player, float dt_seconds) {
    if (!player->base.is_grounded) {
        player->flight_timer += dt_seconds;
    } else {
        player->flight_timer = 0.0f;
    }
}

void player_hammer_update(player_t *player, float dt_seconds) {
    if (player->has_hammer) {
        player->hammer_time -= dt_seconds;
        if (player->hammer_time <= 0.0f) {
            player->has_hammer = false;
            player->hammer_time = 0.0f;
        }
    }
}

void player_check_death(game_state_t *game_state) {
    player_t *player = &game_state->level.player;
    if (player->is_dead) {
        game_state->mode = GAME_MODE_CUTSCENE;
        game_state->current_cutscene = CUTSCENE_MARIO_DEATH;
    } 
    if (game_state->player_lives <= 0) {
        game_state->mode = GAME_MODE_GAME_OVER;
    }
}

void player_movement(game_state_t *game_state, float dt_seconds, float screen_width) {
    player_t *player = &game_state->level.player;

    float move_amount = MOVE_SPEED * dt_seconds;

    bool key_left = is_key_pressed(game_state, GDK_KEY_a) || is_key_pressed(game_state, GDK_KEY_A);
    bool key_right = is_key_pressed(game_state, GDK_KEY_d) || is_key_pressed(game_state, GDK_KEY_D);
    bool key_up = is_key_pressed(game_state, GDK_KEY_w) || is_key_pressed(game_state, GDK_KEY_W);
    bool key_down = is_key_pressed(game_state, GDK_KEY_s) || is_key_pressed(game_state, GDK_KEY_S);
    bool key_space = is_key_pressed(game_state, GDK_KEY_space);
    
    float player_bottom = player->base.y + player->base.height;
    geometry_t *ladder = NULL;
    int current_ladder = player->current_ladder_index;
    if (current_ladder <= game_state->level.num_ladders && game_state->level.ladders) {
        ladder = &game_state->level.ladders[current_ladder];
    }

    if (key_left) {
        player->base.x -= move_amount;
        player->base.direction = -1;
    }
    if (key_right) {
        player->base.x += move_amount;
        player->base.direction = 1;
    }
    if (key_up && player->on_ladder && ladder) {
        float ladder_top = ladder->base.y - LADDER_TOP_OVERLAP;
        if ((player_bottom - EPSILON_2) >= ladder_top) {
            player->base.y -= move_amount;
        }
    }
    if (key_down && player->on_ladder && ladder) {
        float ladder_bottom = ladder->base.y + ladder->base.height;
        if (player_bottom <= ladder_bottom - EPSILON_2) {
            player->base.y += move_amount;
        }
    }
    if (key_space) {
        entity_jump(&player->base, JUMP_FORCE);
    }
}

// determines and sets the appropriate animation state for the player 
void player_change_animation(game_state_t *game_state, float dt_seconds) {
    player_t *player = &game_state->level.player;

    bool key_left = is_key_pressed(game_state, GDK_KEY_a) || is_key_pressed(game_state, GDK_KEY_A);
    bool key_right = is_key_pressed(game_state, GDK_KEY_d) || is_key_pressed(game_state, GDK_KEY_D);
    bool key_up = is_key_pressed(game_state, GDK_KEY_w) || is_key_pressed(game_state, GDK_KEY_W);
    bool key_down = is_key_pressed(game_state, GDK_KEY_s) || is_key_pressed(game_state, GDK_KEY_S);

    float player_bottom = player->base.y + player->base.height;

    geometry_t *ladder = NULL;
    int current_ladder = player->current_ladder_index;
    if (current_ladder <= game_state->level.num_ladders && game_state->level.ladders) {
        ladder = &game_state->level.ladders[current_ladder];
    }
    
    float ladder_top = 0;
    float ladder_bottom = 0;
    if (ladder) {
        ladder_top = ladder->base.y - LADDER_TOP_OVERLAP + EPSILON_4;
        ladder_bottom = ladder->base.y + ladder->base.height - EPSILON_2; 
    }

    if (player->has_hammer && (key_right || key_left)) {
        player->climbing = false;
        set_animation(&player->base, ANIM_HAMMER_MARIO_WALK);
    }
    else if (player->has_hammer && !(key_right || key_left)) {
        player->climbing = false;
        set_animation(&player->base, ANIM_HAMMER_MARIO_STAND);
    }
    else if (!player->base.is_grounded && player->on_elevator && player->flight_timer <= 0.15) {}
    else if (!player->base.is_grounded && !player->on_ladder) {
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