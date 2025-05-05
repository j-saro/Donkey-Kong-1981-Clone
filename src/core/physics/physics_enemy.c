#include "core/physics/physics_enemy.h"
#include "core/physics/physics_utils.h"
#include "entities/abstract/enemy.h"
#include "entities/abstract/effect.h"
#include "core/sprite/animation.h"
#include <math.h>

void enemy_physics(game_state_t *game_state, float dt_seconds);
void enemy_movement(enemy_t *enemy, float dt_seconds);
void enemy_platform_collision(level_t *level, enemy_t *enemy);
void enemy_ladder_option(level_t *level, enemy_t *enemy);
void enemy_player_collision(player_t *player, enemy_t *enemy);
bool enemy_should_be_destroyed_by_player(player_t *player, enemy_t *enemy);

void enemy_physics(game_state_t *game_state, float dt_seconds) {
    level_t *level = &game_state->level;
    // Gravitation Enemies
    for (int i = 0; i < level->num_enemies; i++) {
        enemy_t *enemy = &level->enemies[i];

        // Remove out of bound enemies
        bool hit_with_hammer = enemy_should_be_destroyed_by_player(&level->player, enemy);
        if (enemy->base.x < 0 || enemy->base.x > BASE_WIDTH || hit_with_hammer) {
            if (hit_with_hammer) {
                new_effect(level, ANIM_ENEMY_DEATH, enemy->base.x, enemy->base.y - 10, 1);
                game_state->mode = GAME_MODE_EFFECT;
            }
            enemy_destroy(level, i);
            i--;
            continue;
        }

        enemy_player_collision(&level->player, enemy);
        enemy_movement(enemy, dt_seconds);

        enemy_platform_collision(level, enemy);
        enemy_ladder_option(level, enemy);
        
        if (enemy->fly_time > 0.1 && enemy->base.is_grounded && !enemy->jumping) {
            enemy->base.direction *= -1;
            enemy->fly_time = 0;
            if (!(enemy->base.animation.current_animation == ANIM_BARREL_SIDE)) {
                set_animation(&enemy->base, ANIM_BARREL_SIDE);
            }
        }
    }
}

void enemy_movement(enemy_t *enemy, float dt_seconds) {
    float no_jump_zone = 100;
    if (enemy->base.is_grounded && 
        (rand() % 300) < 2 && 
        enemy->base.x > no_jump_zone && 
        enemy->base.x < (BASE_WIDTH - no_jump_zone) &&
        enemy->base.y > 200) {
        entity_jump(&enemy->base, JUMP_FORCE);
        enemy->jumping = true;
    }

    if (enemy->base.is_grounded) {
        enemy->jumping = false;
    }

    if (enemy->base.velocity_y == 0 || enemy->jumping) {
        enemy->base.x += 130 * enemy->base.direction * dt_seconds;
    } 
    else {
        enemy->base.x += 0.1 * enemy->base.direction;
    }        
    enemy->base.velocity_y += GRAVITY * dt_seconds;
    enemy->base.y += enemy->base.velocity_y * dt_seconds;

    if (!enemy->base.is_grounded && !enemy->jumping) {
        enemy->fly_time += dt_seconds;
    }
}

void enemy_platform_collision(level_t *level, enemy_t *enemy) {
    float enemy_top = enemy->base.y;
    float enemy_bottom = enemy->base.y + PHYSICS_EPSILON;
    float enemy_left = enemy->base.x;
    float enemy_right = enemy->base.x + BASIC_TILE_SIZE;

    bool grounded = false;

    for (int i = 0; i < level->num_platforms; i++) {
        const geometry_t *platform = &level->platforms[i];

        if (!platform->has_physics)
            continue;

        float platform_left = platform->base.x;
        float platform_right = platform_left + platform->base.width;
        float platform_top = platform->base.y;

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

void enemy_ladder_option(level_t *level, enemy_t *enemy) {
    if (enemy->base.is_grounded) {
        float enemy_center_x = enemy->base.x + BASIC_TILE_SIZE * 0.5f;
    
        for (int j = 0; j < level->num_ladders; j++) {
            const geometry_t *ladder = &level->ladders[j];

            if (!ladder->has_physics || ladder->is_cutscene_entity) {
                continue;
            }

            float ladder_center = ladder->base.x - 5;
            float ladder_left = ladder->base.x;
            float ladder_right = ladder_left + ladder->base.width;
            float ladder_top = ladder->base.y - PHYSICS_EPSILON;

            float barrel_bottom = enemy->base.y + 10;
    
            bool aligned_with_ladder = (enemy_center_x > ladder_left && enemy_center_x < ladder_right);
            bool close_enough_y = fabs(barrel_bottom - ladder_top) <= 6.0f;
            
            if (aligned_with_ladder && close_enough_y) {
                int probability = (level->player.current_ladder_index == j) ? 20 : 1;
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
    if (player_object_collision(player, &enemy->base, BASIC_TILE_SIZE, 10)) {
        player->has_hammer = false;
        player->is_dead = true;
    }
}

bool enemy_should_be_destroyed_by_player(player_t *player, enemy_t *enemy) {
    bool collision = player_object_collision(player, &enemy->base, BASIC_TILE_SIZE, 10) && player->has_hammer;
    
    bool right_direction = true;
    if (enemy->base.y > player->base.y) {
        right_direction = player->base.direction * enemy->base.direction < 0;
    }

    return collision && right_direction;
}
