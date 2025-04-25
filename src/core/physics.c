#include "core/physics.h"
#include <stdio.h>
#include "entities/abstract/enemy.h"
#include <math.h>
#include <stdlib.h>
#include "core/animation.h"


void apply_physics(game_state_t *game_state, float dt_seconds, float screen_height);
void window_collision(game_state_t *game_state, float screen_height);
void platform_collision(game_state_t *game_state);
void check_ladder_collision(game_state_t *game_state);
void enemy_platform_collision(level_t *level, enemy_t *enemy);
void enemy_physics(level_t *level, float dt_seconds);
void enemy_ladder_option(level_t *level, enemy_t *enemy);
void enemy_player_collision(player_t *player, enemy_t *enemy);

void apply_physics(game_state_t *game_state, float dt_seconds, float screen_height) {
    player_t *player = &game_state->level.player;

    // Gravitation Player
    if (!player->climbing) {
        player->base.velocity_y += GRAVITY * dt_seconds;
        player->base.y += player->base.velocity_y * dt_seconds;
    }

    window_collision(game_state, screen_height);
    platform_collision(game_state);
    enemy_physics(&game_state->level, dt_seconds);
}

void window_collision(game_state_t *game_state, float screen_height) {
    player_t *player = &game_state->level.player;

    float player_bottom = player->base.y + PLAYER_HEIGHT;

    // Left/Right window collison
    if (player->base.x < 0) {
        player->base.x = 0;
    }
    if (player->base.x > BASE_WIDTH - PLAYER_WIDTH) {
        player->base.x = BASE_WIDTH - PLAYER_WIDTH;
    }
    
    // Bottom window collision
    if (player_bottom > screen_height) {
        player->base.y = screen_height - PLAYER_HEIGHT;
        player->base.velocity_y = 0;
        player->base.is_grounded = true;
    } else {
        player->base.is_grounded = false;
    }
}

void platform_collision(game_state_t *game_state) {
    player_t *player = &game_state->level.player;

    float old_bottom = player->previous_y + PLAYER_HEIGHT;
    float new_bottom = player->base.y + PLAYER_HEIGHT;
    float player_left = player->base.x;
    float player_right = player->base.x + PLAYER_WIDTH;

    bool grounded = false;
    for (int i = 0; i < game_state->level.num_platforms; i++) {
        const geometry_t *platform = &game_state->level.platforms[i];

        if (!platform->has_physics)
            continue;

        float platform_left = platform->x;
        float platform_right = platform_left + platform->width;
        float platform_top = platform->y;

        // calc x-overlap
        if (!(player_right > platform_left && player_left < platform_right))
            continue;

        // snap player to platform
        if (player->base.velocity_y > 0 &&
            (old_bottom - 2) <= platform_top &&
            new_bottom >= platform_top)
        {
            player->base.y = platform_top - PLAYER_HEIGHT;
            player->base.velocity_y = 0;
            player->current_platform_index = i;
            grounded = true;
            break;
        }
    }
    player->base.is_grounded = grounded;
}

void check_ladder_collision(game_state_t *game_state) {
    player_t *player = &game_state->level.player;
    float player_center = player->base.x + PLAYER_WIDTH * 0.5f;
    float player_top = player->base.y;
    float player_bottom = player->base.y + PLAYER_HEIGHT;

    bool is_on_ladder = false;

    for (int i = 0; i < game_state->level.num_ladders; i++) {
        const geometry_t *ladder = &game_state->level.ladders[i];

        if (!ladder->has_physics || player->base.animation.current_animation == ANIM_JUMP_MARIO) {
            continue;
        }

        float ladder_left = ladder->x;
        float ladder_right = ladder_left + ladder->width;
        float ladder_top = ladder->y - PHYSICS_EPSILON;
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

void enemy_platform_collision(level_t *level, enemy_t *enemy) {
    float enemy_top = enemy->base.y;
    float enemy_bottom = enemy->base.y + PHYSICS_EPSILON;
    float enemy_left = enemy->base.x;
    float enemy_right = enemy->base.x + TILE_SIZE;

    bool grounded = false;

    for (int i = 0; i < level->num_platforms; i++) {
        const geometry_t *platform = &level->platforms[i];

        if (!platform->has_physics)
            continue;

        float platform_left = platform->x;
        float platform_right = platform_left + platform->width;
        float platform_top = platform->y;

        // Check horizontal overlap
        if (!(enemy_right >= platform_left && enemy_left <= platform_right))
            continue;

        // Check vertical collision (enemy falling onto platform)
        if (enemy->base.velocity_y >= 0 &&
            (enemy_bottom + 2) >= platform_top &&
            enemy_top < platform_top)
        {
            enemy->base.y = platform_top - PHYSICS_EPSILON;
            enemy->base.velocity_y = 0;
            grounded = true;
            break;
        }
    }

    enemy->base.is_grounded = grounded;
}

void enemy_physics(level_t *level, float dt_seconds) {
    // Gravitation Enemies
    for (int i = 0; i < level->num_enemies; i++) {
        enemy_t *enemy = &level->enemies[i];

        enemy_player_collision(&level->player, enemy);

        // Apply movement
        if (enemy->base.velocity_y == 0) {
            enemy->base.x += MOVE_SPEED * enemy->base.direction * dt_seconds;
        } 
        else {

            enemy->base.x += 0.1 * enemy->base.direction;
        }        
        enemy->base.velocity_y += GRAVITY * dt_seconds;
        enemy->base.y += enemy->base.velocity_y * dt_seconds;

        if (!enemy->base.is_grounded) {
            enemy->fly_time += dt_seconds;
        }

        enemy_platform_collision(level, enemy);
        enemy_ladder_option(level, enemy);
        
        if (enemy->fly_time > 0.1 && enemy->base.is_grounded) {
            enemy->base.direction *= -1;
            enemy->fly_time = 0;
            if (!(enemy->base.animation.current_animation == ANIM_BARREL_SIDE)) {
                set_animation(&enemy->base, ANIM_BARREL_SIDE);
            }
        }

        // Remove out of bound enemies
        if (enemy->base.x < 0 || enemy->base.x > BASE_WIDTH) {
            enemy_destroy(level, i);
            i--;
            continue;
        }
    }
}

void enemy_ladder_option(level_t *level, enemy_t *enemy) {
    if (enemy->base.is_grounded) {
        float enemy_center_x = enemy->base.x + TILE_SIZE * 0.5f;
    
        for (int j = 0; j < level->num_ladders; j++) {
            const geometry_t *ladder = &level->ladders[j];

            if (!ladder->has_physics) {
                continue;
            }

            float ladder_center = ladder->x - 5;
            float ladder_left = ladder->x;
            float ladder_right = ladder_left + ladder->width;
            float ladder_top = ladder->y - PHYSICS_EPSILON;

            float barrel_bottom = enemy->base.y + 10;
    
            bool aligned_with_ladder = (enemy_center_x > ladder_left && enemy_center_x < ladder_right);
            bool close_enough_y = fabs(barrel_bottom - ladder_top) <= 6.0f;
            
            if (aligned_with_ladder && close_enough_y) {
                int probability = (level->player.current_ladder_index == j) ? 20 : 2;
                if ((rand() % 200) < probability) {
                    enemy->base.y += 30;
                    enemy->base.is_grounded = false;
                    set_animation(&enemy->base, ANIM_BARREL_FRONT);
                    break;
                }
            }
        }
    }
}

void enemy_player_collision(player_t *player, enemy_t *enemy) {
    float enemy_left = enemy->base.x;
    float enemy_right = enemy->base.x + TILE_SIZE;
    float enemy_top = enemy->base.y;
    float enemy_bottom = enemy->base.y + 10;

    float player_left = player->base.x;
    float player_right = player->base.x + PLAYER_WIDTH;
    float player_top = player->base.y;
    float player_bottom = player->base.y + PLAYER_HEIGHT;

    bool collision = !(player_right < enemy_left ||
                       player_left > enemy_right ||
                       player_bottom < enemy_top ||
                       player_top > enemy_bottom);

    if (collision) {
        player->base.x = 120;
        player->base.y = 530;
    }
}