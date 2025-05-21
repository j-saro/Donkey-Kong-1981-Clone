#include <math.h>
#include "consts.h"
#include "core/physics/physics_enemy.h"
#include "core/physics/physics_utils.h"
#include "entities/abstract/enemy.h"
#include "entities/abstract/effect.h"
#include "core/sprite/animation.h"

void enemy_physics(game_state_t *game_state, float dt_seconds);
void barrel_movement(enemy_t *enemy, float dt_seconds);
void fire_spirit_movement(level_t *level, enemy_t *enemy, float dt_seconds);
void lore_movement(enemy_t *enemy, float dt_seconds);
void spring_movement(enemy_t *enemy, float dt_seconds);
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
        if (enemy->base.x < 0 || 
            enemy->base.x > BASE_WIDTH || 
            enemy->base.y > BASE_HEIGHT || 
            enemy->base.y < 0 || 
            hit_with_hammer) {
            if (hit_with_hammer) {
                game_state->player_score += ENEMY_POINTS;
                new_effect(level, ANIM_ENEMY_DEATH, enemy->base.x, enemy->base.y, 1, true);
                new_effect(level, ANIM_300_POINTS, enemy->base.x + 30, enemy->base.y - 20, 1, true);
                game_state->mode = GAME_MODE_EFFECT;
            }
            enemy_destroy(level, i);
            i--;
            continue;
        }

        enemy_player_collision(&level->player, enemy);

        // set enemy type specific movement
        switch (enemy->base.animation.current_animation) {
            case ANIM_BARREL_FRONT:
            case ANIM_BARREL_SIDE:
                barrel_movement(enemy, dt_seconds);
                break;
            case ANIM_FIRE_SPIRIT_WALK:
            case ANIM_FIRE_GHOST_WALK:
                fire_spirit_movement(&game_state->level, enemy, dt_seconds);
                break;
            case ANIM_IDLE_LORE:
                lore_movement(enemy, dt_seconds);
                break;
            case ANIM_SPRING:
                spring_movement(enemy, dt_seconds);
                continue;
        }

        enemy_platform_collision(level, enemy);
        enemy_ladder_option(level, enemy);
        
        // if enemy falls of platform or climbs ladder, change direction
        if (enemy->fly_time > 0.1 && enemy->base.is_grounded && !enemy->jumping) {
            enemy->base.direction *= -1;
            enemy->fly_time = 0;

            if (!(enemy->base.animation.current_animation == ANIM_BARREL_SIDE)) {
                set_animation(&enemy->base, ANIM_BARREL_SIDE);
            }
            if (enemy->base.animation.current_animation == ANIM_FIRE_SPIRIT_WALK ||
                enemy->base.animation.current_animation == ANIM_FIRE_GHOST_WALK) {
              enemy->base.x += ENEMY_OFFSET;
          }
        }
    }
}

void barrel_movement(enemy_t *enemy, float dt_seconds) {
    float no_jump_zone = 100;
    // randomly jump
    if (enemy->base.is_grounded && 
        (rand() % 300) < 2 && 
        enemy->base.x > no_jump_zone && 
        enemy->base.x < (BASE_WIDTH - no_jump_zone) &&
        enemy->base.y > no_jump_zone * 2) {
        entity_jump(&enemy->base, JUMP_FORCE);
        enemy->jumping = true;
    }

    if (enemy->base.is_grounded) {
        enemy->jumping = false;
    }

    // Normal Movement Speed
    if (enemy->base.velocity_y == 0 || enemy->jumping) {
        enemy->base.x += BARREL_MOVE_SPEED * enemy->base.direction * dt_seconds;
    }
    // if falling, reduce movement 
    else {
        enemy->base.x += 0.1 * enemy->base.direction;
    }

    // if falling, update fly time
    if (!enemy->base.is_grounded && !enemy->jumping) {
        enemy->fly_time += dt_seconds;
    }

    // Gravity
    enemy->base.velocity_y += GRAVITY * dt_seconds;
    enemy->base.y += enemy->base.velocity_y * dt_seconds;
}

void fire_spirit_movement(level_t *level, enemy_t *enemy, float dt_seconds) {
    // climb ladder up/down
    if (enemy->on_ladder && !enemy->base.is_grounded) {
        enemy->base.y += FIRE_SPIRIT_MOVE_SPEED * dt_seconds * enemy->climb_direction;
        return;
    }

    // dont fall of platforms
    if (enemy->base.velocity_y > 0) {
        enemy->base.direction *= -1;
        enemy->base.x += ENEMY_OFFSET * enemy->base.direction;
        return;
    }

    // change randomly direction
    if (enemy->base.x > BASE_WIDTH - (enemy->base.width / 2.0)) {
        enemy->base.x = BASE_WIDTH - enemy->base.width;
    }

    if (((rand() % 200) < 1 || 
        enemy->base.x < enemy->base.width || 
        enemy->base.x < 0 ||
        enemy->base.x > BASE_WIDTH - enemy->base.width) && 
        level->frame_timer < 0.5) {
        enemy->base.direction *= -1;
    }

    enemy->base.x += (MOVE_SPEED / 1.5f) * enemy->base.direction * dt_seconds;

    // only gravity when not on ladder
    if (!enemy->on_ladder) {
        enemy->base.velocity_y += GRAVITY * dt_seconds;
        enemy->base.y += enemy->base.velocity_y * dt_seconds;
    }
}

void lore_movement(enemy_t *enemy, float dt_seconds) {
    enemy->base.x += (MOVE_SPEED / 1.5f) * enemy->base.direction * dt_seconds;
}

void spring_movement(enemy_t *enemy, float dt_seconds) {
    entity_jump(&enemy->base, JUMP_FORCE);

    if (enemy->base.x < enemy->stop_x_threshold) {
        enemy->base.x += SPRING_MOVE_SPEED * dt_seconds;
    }

    if (enemy->base.y > (ENEMY_Y_PLATFORM - enemy->base.height) && enemy->base.x < ENEMY_X_PLATFORM) {
        enemy->base.is_grounded = true;
    }
    
    // Gravity
    enemy->base.velocity_y += GRAVITY * dt_seconds;
    enemy->base.y += enemy->base.velocity_y * dt_seconds;
}

void enemy_platform_collision(level_t *level, enemy_t *enemy) {
    float enemy_top = enemy->base.y;
    float enemy_bottom = enemy->base.y + enemy->base.height;
    float enemy_left = enemy->base.x;
    float enemy_right = enemy->base.x + BASIC_TILE_SIZE;

    bool grounded = false;

    for (int i = 0; i < level->num_platforms; i++) {
        const geometry_t *platform = &level->platforms[i];

        if (!platform->has_physics) continue;

        float platform_left = platform->base.x;
        float platform_right = platform_left + platform->base.width;
        float platform_top = platform->base.y;

        // Check horizontal overlap
        if (!(enemy_right >= platform_left && enemy_left <= platform_right))
            continue;

        // Check vertical collision (enemy falling onto platform)
        if (enemy->base.velocity_y >= 0 &&
            (enemy_bottom + EPSILON_2) >= platform_top &&
            enemy_top < platform_top)
        {
            enemy->base.y = platform_top - enemy->base.height;
            enemy->base.velocity_y = 0;
            grounded = true;
            break;
        }
    }

    enemy->base.is_grounded = grounded;
}

void enemy_ladder_option(level_t *level, enemy_t *enemy) {
    if (enemy->base.is_grounded) {
        float enemy_center_x = enemy->base.x + enemy->base.width * 0.5f;
    
        for (int j = 0; j < level->num_ladders; j++) {
            const geometry_t *ladder = &level->ladders[j];

            if (!ladder->has_physics ||
                ladder->base.animation.current_animation == ANIM_ELEVATOR_LINE ||
                ladder->base.animation.current_animation == ANIM_POLE_BLUE) {
                continue;
            }

            float ladder_center = ladder->base.x;
            float ladder_left = ladder->base.x;
            float ladder_right = ladder_left + ladder->base.width;
            float ladder_top = ladder->base.y - LADDER_TOP_OVERLAP;
            float ladder_bottom = ladder->base.y + ladder->base.height;

            float enemy_bottom = enemy->base.y + enemy->base.height;
    
            bool aligned_with_ladder = (enemy_center_x > ladder_left && enemy_center_x < ladder_right);
            bool close_enough_top_y = fabs(enemy_bottom - ladder_top) <= 6.0f;
            bool close_enough_bottom_y = fabs(ladder_bottom - enemy_bottom) <= 6.0f;

            if (!aligned_with_ladder) {
                enemy->on_ladder = false;
            }

            // if aligned with ladder
            if (aligned_with_ladder && (close_enough_top_y || close_enough_bottom_y)) {

                int probability = (level->player.current_ladder_index == j) ? 3 : 1;
                int rand_int = rand() % 200;

                switch (enemy->base.animation.current_animation) {
                    case ANIM_BARREL_FRONT:
                    case ANIM_BARREL_SIDE:
                        if (rand_int < probability)
                            // if above ladder, climb down
                            if (close_enough_top_y) {
                                enemy->base.y += 30;
                                enemy->base.is_grounded = false;
                                enemy->on_ladder = true;
                                set_animation(&enemy->base, ANIM_BARREL_FRONT);
                            }
                        break;
                    case ANIM_FIRE_SPIRIT_WALK:
                    case ANIM_FIRE_GHOST_WALK:
                        // if abouve ladder, climp down
                        if (close_enough_top_y && rand_int < probability) {
                            enemy->climb_direction = 1;
                            enemy->base.y += FIRE_SPIRIT_MOVE_SPEED;
                            enemy->on_ladder = true;
                            enemy->base.is_grounded = false;
                        }
                        // if at bottom of ladder, climb up 
                        else if (close_enough_bottom_y && rand_int < probability) {
                            enemy->climb_direction = -1;
                            enemy->base.y -= FIRE_SPIRIT_MOVE_SPEED;
                            enemy->on_ladder = true;
                            enemy->base.is_grounded = false;
                        }
                        break;
                }
                break;
            }
        }
    }
}

void enemy_player_collision(player_t *player, enemy_t *enemy) {
    if (player_object_collision(player, &enemy->base, enemy->base.width, enemy->base.height)) {
        player->has_hammer = false;
        player->is_dead = true;
    }
}

bool enemy_should_be_destroyed_by_player(player_t *player, enemy_t *enemy) {
    bool collision = player_object_collision(player, &enemy->base, enemy->base.width, enemy->base.height) && player->has_hammer;
    
    bool right_direction = true;
    if (enemy->base.y > player->base.y) {
        // player is facing enemy
        if (enemy->base.animation.current_animation == ANIM_BARREL_FRONT || enemy->base.animation.current_animation == ANIM_BARREL_SIDE) {
            right_direction = player->base.direction * enemy->base.direction < 0;
        }
    }

    return collision && right_direction;
}
