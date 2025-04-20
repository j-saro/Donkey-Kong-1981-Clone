#include "animation.h"

const AnimationSequence animations[] = {
    [ANIM_IDLE] = {1, 21, 1, 0.2f},
    [ANIM_WALK] = {1, 21, 3, 0.1f},
    [ANIM_JUMP] = {63, 21, 1, 0.15f},
    [ANIM_CLIMB] = {132, 21, 2, 0.1f},
    //[ANIM_CLIMB_TOP] = {164, 21, 3, 0.5f},
    [ANIM_CLIMB_IDLE] = {132, 21, 1, 1.0f},
};

void update_player_animation_state(game_state_t *game_state, float dt_seconds);
void update_player_animation(player_t *player, float dt_seconds);
void update_animation_frame(player_t *player);
void set_player_animation(player_t *player, AnimationState new_anim);

// determines and sets the appropriate animation state for the player 
void update_player_animation_state(game_state_t *game_state, float dt_seconds) {
    player_t *player = &game_state->player;
    bool key_left   = game_state->pressed_keys['a'] || game_state->pressed_keys['A'];
    bool key_right  = game_state->pressed_keys['d'] || game_state->pressed_keys['D'];
    bool key_up     = game_state->pressed_keys['w'] || game_state->pressed_keys['W'];
    bool key_down   = game_state->pressed_keys['s'] || game_state->pressed_keys['S'];

    if (!player->is_grounded && !player->on_ladder) {
        player->climbing = false;
        set_player_animation(player, ANIM_JUMP);
        return;
    }
    if (player->on_ladder && (key_up || key_down)) {
        player->climbing = true;
        set_player_animation(player, ANIM_CLIMB);
        return;
    }
    if (player->on_ladder && player->climbing) {
        set_player_animation(player, ANIM_CLIMB_IDLE);
        return;
    }
    if (player->is_grounded && (key_left || key_right)) {
        player->climbing = false;
        set_player_animation(player, ANIM_WALK);
        return;
    }

    player->climbing = false;
    set_player_animation(player, ANIM_IDLE);
}

// keeps the animation frame_time updated
void update_player_animation(player_t *player, float dt_seconds) {
    AnimationSequence anim = animations[player->current_animation];
    
    player->frame_time += dt_seconds;
    
    if (player->frame_time >= anim.frame_duration) {
        player->frame_time = 0;
        player->current_frame_index = (player->current_frame_index + 1) % anim.frame_count;
        update_animation_frame(player);
    }
}

// changes to next frame in the animation
void update_animation_frame(player_t *player) {
    if (player->current_frame != NULL) {
        cairo_surface_destroy(player->current_frame);
        player->current_frame = NULL;
    }
    
    AnimationSequence anim = animations[player->current_animation];
    
    int x = anim.start_x + (player->current_frame_index * player->frame_width);
    int y = anim.start_y;
    
    player->current_frame = cairo_surface_create_for_rectangle(
        player->sprite_sheet,
        x, y,
        player->frame_width,
        player->frame_height
    );
    
    if (cairo_surface_status(player->current_frame) != CAIRO_STATUS_SUCCESS) {
        g_warning("Failed to create frame surface at (%d,%d)", x, y);
    }
}

// sets a new Animation
void set_player_animation(player_t *player, AnimationState new_anim) {
    if (player->current_animation != new_anim) {
        player->current_animation = new_anim;
        player->current_frame_index = 0;
        player->frame_time = 0;
        update_animation_frame(player);
    }
}