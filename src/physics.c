#include "physics.h"

void apply_physics(game_state_t *game_state, float dt_seconds, float screen_height) {
    game_state->player.velocity_y += GRAVITY * dt_seconds;

    game_state->player.y += game_state->player.velocity_y * dt_seconds;
    
    float player_bottom = game_state->player.y + PLAYER_HEIGHT;
    if (player_bottom > screen_height - GROUND_LEVEL) {
        game_state->player.y = screen_height - GROUND_LEVEL - PLAYER_HEIGHT;
        game_state->player.velocity_y = 0;
        game_state->player.is_grounded = true;
    } else {
        game_state->player.is_grounded = false;
    }
}