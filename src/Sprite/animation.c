#include "animation.h"

const AnimationSequence animations[] = {
    [ANIM_IDLE] = {1, 21, 1, 0.2f},
    [ANIM_WALK] = {1, 21, 3, 0.1f},
    [ANIM_JUMP] = {63, 21, 1, 0.15f},
};

void update_player_animation_state(game_state_t *game_state, float dt_seconds);
void update_player_animation(player_t *player, float dt_seconds);
void update_animation_frame(player_t *player);
void set_player_animation(player_t *player, AnimationState new_anim);

// determines and sets the appropriate animation state for the player 
void update_player_animation_state(game_state_t *game_state, float dt_seconds) {
    if (!game_state->player.is_grounded) {
        set_player_animation(&game_state->player, ANIM_JUMP);
    } 
    else if (game_state->pressed_keys['a'] || game_state->pressed_keys['A'] || 
             game_state->pressed_keys['d'] || game_state->pressed_keys['D']) {
        set_player_animation(&game_state->player, ANIM_WALK);
    }
    else {
        set_player_animation(&game_state->player, ANIM_IDLE);
    }
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