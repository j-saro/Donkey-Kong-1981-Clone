#include "physics.h"
#include <stdio.h>

void apply_physics(game_state_t *game_state, float dt_seconds, float screen_height);
void window_collision(game_state_t *game_state, float screen_height);
void platform_collision(game_state_t *game_state);

void apply_physics(game_state_t *game_state, float dt_seconds, float screen_height) {
    // Gravitation
    game_state->player.velocity_y += GRAVITY * dt_seconds;
    game_state->player.y += game_state->player.velocity_y * dt_seconds;

    window_collision(game_state, screen_height);
    platform_collision(game_state);
}

void window_collision(game_state_t *game_state, float screen_height) {
    player_t *player = &game_state->player;

    float player_bottom = player->y + PLAYER_HEIGHT;

    // Left/Right window collison
    if (player->x < 0) {
        player->x = 0;
    }
    if (player->x > BASE_WIDTH - PLAYER_WIDTH) {
        player->x = BASE_WIDTH - PLAYER_WIDTH;
    }
    
    // Bottom window collision
    if (player_bottom > screen_height) {
        player->y = screen_height - PLAYER_HEIGHT;
        player->velocity_y = 0;
        player->is_grounded = true;
    } else {
        player->is_grounded = false;
    }
}

void platform_collision(game_state_t *game_state) {
    player_t *player = &game_state->player;

    float old_bottom = player->previous_y + PLAYER_HEIGHT;
    float new_bottom = player->y + PLAYER_HEIGHT;
    float player_left = player->x;
    float player_right = player->x + PLAYER_WIDTH;

    bool grounded = false;
    for (int i = 0; i < game_state->level.num_platforms; i++) {
        const structure_t *platform = &game_state->level.platforms[i];

        float platform_left = platform->position.x;
        float platform_right = platform_left + platform->width;
        float platform_top = platform->position.y;

        // calc x-overlap
        if (!(player_right > platform_left && player_left < platform_right))
            continue;

        if (player->velocity_y > 0 &&
            (old_bottom - 2) <= platform_top &&
            new_bottom >= platform_top)
        {
            player->y = platform_top - PLAYER_HEIGHT;
            player->velocity_y = 0;
            grounded = true;
        }
    }
    player->is_grounded = grounded;
}
