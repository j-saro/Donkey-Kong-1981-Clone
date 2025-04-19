#include "physics.h"
#include <stdio.h>

void apply_physics(game_state_t *game_state, float dt_seconds, float screen_height);

void apply_physics(game_state_t *game_state, float dt_seconds, float screen_height) {
    game_state->player.velocity_y += GRAVITY * dt_seconds;
    game_state->player.y += game_state->player.velocity_y * dt_seconds;

    float player_top = game_state->player.y;
    float player_bottom = game_state->player.y + PLAYER_HEIGHT;
    float player_left = game_state->player.x;
    float player_right = game_state->player.x + PLAYER_WIDTH;

    // Ground collision
    if (player_bottom > screen_height - GROUND_LEVEL) {
        game_state->player.y = screen_height - GROUND_LEVEL - PLAYER_HEIGHT;
        game_state->player.velocity_y = 0;
        game_state->player.is_grounded = true;
    } else {
        game_state->player.is_grounded = false;
    }

    // Platform collision
    for (int i = 0; i < game_state->level.num_platforms; i++) {
        const platform_t *platform = &game_state->level.platforms[i];
    
        float platform_left = platform->position.x;
        float platform_right = platform->position.x + platform->width;
        float platform_top = platform->position.y;
        float platform_bottom = platform->position.y + platform->height;
    
        // Check for horizontal overlap
        bool horizontally_overlapping = player_right > platform_left && player_left < platform_right;
        
        bool is_falling = game_state->player.velocity_y > 0;
        bool is_jumping = game_state->player.velocity_y < 0;

        if (horizontally_overlapping && 
            is_falling && 
            player_bottom >= platform_top && 
            player_top < platform_top) {

            game_state->player.y = platform_top - PLAYER_HEIGHT;
            game_state->player.velocity_y = 0;
            game_state->player.is_grounded = true;
        }

       // stop form jumping through the bottom of an platform
        if (horizontally_overlapping && is_jumping && player_top <= platform_bottom && player_bottom > platform_bottom) {
            game_state->player.y = platform_bottom;
            game_state->player.velocity_y = 0;
        }
    }
}
