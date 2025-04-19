#include "physics.h"

void apply_physics(game_state_t *game_state, float dt_seconds, float screen_height);

void apply_physics(game_state_t *game_state, float dt_seconds, float screen_height) {
    game_state->player.velocity_y += GRAVITY * dt_seconds;
    game_state->player.y += game_state->player.velocity_y * dt_seconds;
    float player_bottom = game_state->player.y + PLAYER_HEIGHT;
    float player_top = game_state->player.y - PLAYER_HEIGHT;

    // ground collision
    if (player_bottom > screen_height - GROUND_LEVEL) {
        game_state->player.y = screen_height - GROUND_LEVEL - PLAYER_HEIGHT;
        game_state->player.velocity_y = 0;
        game_state->player.is_grounded = true;
    } else {
        game_state->player.is_grounded = false;
    }

    // platfrom collision
    for (int i = 0; i < game_state->level.num_platforms; i++) {
        const platform_t *platform = &game_state->level.platforms[i];
    
        float platform_left   = platform->position.x;
        float platform_right  = platform->position.x + platform->width;
        float platform_top    = platform->position.y;
        float platform_bottom = platform->position.y + platform->height;
    
        float player_left  = game_state->player.x;
        float player_right = game_state->player.x + PLAYER_WIDTH;
    
        // Check if player's bottom is falling onto the platform's top
        bool horizontally_overlapping = player_right > platform_left && player_left < platform_right;
        bool is_falling = game_state->player.velocity_y > 0;
    
        if (horizontally_overlapping &&
            player_bottom >= platform_top && player_top < platform_top &&
            is_falling)
        {
            // Snap player to the platform
            game_state->player.y = platform_top - PLAYER_HEIGHT;
            game_state->player.velocity_y = 0;
            game_state->player.is_grounded = true;
        }
    }
}