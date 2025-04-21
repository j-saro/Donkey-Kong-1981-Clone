#include "physics.h"
#include <stdio.h>

void apply_physics(game_state_t *game_state, float dt_seconds, float screen_height);
void window_collision(game_state_t *game_state, float screen_height);
void platform_collision(game_state_t *game_state);
void check_ladder_collision(game_state_t *game_state);

void apply_physics(game_state_t *game_state, float dt_seconds, float screen_height) {
    // Gravitation
    if (!game_state->player.climbing) {
        game_state->player.velocity_y += GRAVITY * dt_seconds;
        game_state->player.y += game_state->player.velocity_y * dt_seconds;
    }

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

        float platform_left = platform->x;
        float platform_right = platform_left + platform->width;
        float platform_top = platform->y;

        // calc x-overlap
        if (!(player_right > platform_left && player_left < platform_right) || !platform->has_physics)
            continue;

        // snap player to platform
        if (player->velocity_y > 0 &&
            (old_bottom - 2) <= platform_top &&
            new_bottom >= platform_top)
        {
            player->y = platform_top - PLAYER_HEIGHT;
            player->velocity_y = 0;
            player->current_platform_index = i;
            grounded = true;
            break;
        }
    }
    player->is_grounded = grounded;
}

void check_ladder_collision(game_state_t *game_state) {
    player_t *player = &game_state->player;
    float player_center = player->x + PLAYER_WIDTH * 0.5f;
    float player_top = player->y;
    float player_bottom = player->y + PLAYER_HEIGHT;

    bool is_on_ladder = false;

    for (int i = 0; i < game_state->level.num_ladders; i++) {
        const structure_t *ladder = &game_state->level.ladders[i];

        if (!ladder->has_physics || player->animation.current_animation == ANIM_JUMP) {
            continue;
        }

        float ladder_left = ladder->x;
        float ladder_right = ladder_left + ladder->width;
        float ladder_top = ladder->y - LADDER_EXTRA;
        float ladder_bottom = ladder_top + ladder->height;

        bool inside_ladder = (player_center > ladder_left && player_center < ladder_right);
        bool overlap_ladder = (player_bottom > ladder_top && player_top < ladder_bottom);

        if (inside_ladder && overlap_ladder) {
            is_on_ladder = true;
            player->current_ladder_index = i;
            break;
        }
    }
    player->on_ladder = is_on_ladder;
}
