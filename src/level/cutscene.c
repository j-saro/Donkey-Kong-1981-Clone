#include "consts.h"
#include "level/cutscene.h"
#include "level/level.h"
#include "core/sprite/animation.h"
#include "core/sprite/sprite_utils.h"
#include "core/physics/physics.h"
#include "core/physics/physics_utils.h"
#include "core/physics/physics_geometry.h"
#include "entities/abstract/effect.h"
#include "entities/abstract/enemy.h"

void cutscene_load(game_state_t *game_state, float dt_seconds);
void next_cutscene_step(game_state_t *game_state);
void cutscene_finish(game_state_t *game_state);
void cutscene_dk_intro(game_state_t *game_state, float dt_seconds);
void cutscene_dk_escape(game_state_t *game_state, float dt_seconds);
void cutscene_dk_death(game_state_t *game_state, float dt_seconds);
void cutscene_mario_death(game_state_t *game_state, float dt_seconds);

void cutscene_load(game_state_t *game_state, float dt_seconds) {
    switch (game_state->current_cutscene) {
        case CUTSCENE_DK_INTRO:
            cutscene_dk_intro(game_state, dt_seconds);
            break;
        case CUTSCENE_DK_ESCAPE:
            cutscene_dk_escape(game_state, dt_seconds);
            break;
        case CUTSCENE_DK_DEATH:
            cutscene_dk_death(game_state, dt_seconds);
            break;
        case CUTSCENE_MARIO_DEATH:
            cutscene_mario_death(game_state, dt_seconds);
            break;
        default:
            g_warning("Not a valid cutscene: %d", game_state->current_cutscene);
            break;
    }
}

void next_cutscene_step(game_state_t *game_state) {
    game_state->cutscene_time = 0;
    game_state->cutscene_step++;
}

void cutscene_finish(game_state_t *game_state) {
    game_state->cutscene_time = 0;
    game_state->cutscene_step = 0;
    game_state->mode = GAME_MODE_NORMAL;

    if (!level_next(game_state, LEVEL_FILE_PATH, &game_state->current_level)) {
        game_state->mode = GAME_MODE_PAUSED;
        return;
    }
}

void cutscene_dk_intro(game_state_t *game_state, float dt_seconds) {
    donkey_kong_t *donkey_kong = &game_state->level.donkey_kong;
    game_state->cutscene_time += dt_seconds;

    update_animation_progress(&donkey_kong->base, dt_seconds);

    switch (game_state->cutscene_step) {
        case 0:
            // Start cutscene after 1 seconds
            if (game_state->cutscene_time >= 1.0f) {
                set_animation(&donkey_kong->base, ANIM_CLIMB_WITH_PEACH_DONKEY_KONG);
                next_cutscene_step(game_state);
            }
            break;
        case 1:
            // Donkey Kong climbs ladders
            donkey_kong->base.y -= DK_CLIMB_SPEED * dt_seconds;

            if (donkey_kong->base.y <= C1_DK_CLIMB_HEIGHT) {
                set_animation(&donkey_kong->base, ANIM_IDLE_DONKEY_KONG);
                set_animation(&game_state->level.peach.base, ANIM_HELP_PEACH);
                next_cutscene_step(game_state);
            }
            break;

        case 2:
            // Wait for 1 second
            if (game_state->cutscene_time >= 1.0f) {
                next_cutscene_step(game_state);
            }
            break;

        case 3:
            // Donky Kong goes to the left
            donkey_kong->base.x -= MOVE_SPEED * dt_seconds;

            if (donkey_kong->base.x >= C1_DK_CLIMB_HEIGHT - EPSILON_2) {
                entity_jump(&donkey_kong->base, DK_JUMP_FORCE);
            }
            apply_gravity(&donkey_kong->base, dt_seconds, DK_GRAVITY);
            if (donkey_kong->base.y >= C1_DK_CLIMB_HEIGHT) {
                donkey_kong->base.y = C1_DK_CLIMB_HEIGHT;
                donkey_kong->base.is_grounded = true;
            }

            if (donkey_kong->base.x <= C1_DK_FINAL_POS_X) {
                next_cutscene_step(game_state);
            }
            break;

        case 4:
            // End of cutscene - switch back to game mode
            if (game_state->cutscene_time > 1.0f) {
                cutscene_finish(game_state);
            }
            break;
    }
}

void cutscene_dk_escape(game_state_t *game_state, float dt_seconds) {
    donkey_kong_t *donkey_kong = &game_state->level.donkey_kong;

    game_state->cutscene_time += dt_seconds;

    update_animation_progress(&donkey_kong->base, dt_seconds);

    switch (game_state->cutscene_step) {
        case 0:
            if (game_state->level.num_effects == 0) {
                new_effect(&game_state->level, ANIM_HEART_FULL, C2_HEART_X, C2_HEART_Y, 1, false);
            }
            if (game_state->cutscene_time > 1.0f) {
                next_cutscene_step(game_state);
            }
            break;
        case 1:
            set_animation(&donkey_kong->base, ANIM_CLIMB_WITHOUT_PEACH_DONKEY_KONG);
            donkey_kong->base.x = C2_DK_START_X;
            donkey_kong->base.y = C2_DK_START_Y;
            next_cutscene_step(game_state);
            break;
        case 2:
            // Donkey Kong climb to peach
            donkey_kong->base.y -= DK_CLIMB_SPEED * dt_seconds;

            if (donkey_kong->base.y <= game_state->level.peach.base.y) {
                donkey_kong->base.x = C2_DK_START_X - EPSILON_4;
                set_animation(&donkey_kong->base, ANIM_CLIMB_WITH_PEACH_DONKEY_KONG);
                set_animation(&game_state->level.peach.base, ANIM_HIDE);
                effect_destroy(&game_state->level, 0);
                new_effect(&game_state->level, ANIM_HEART_BROKEN, C2_HEART_X, C2_HEART_Y, 1, false);
                next_cutscene_step(game_state);
            }
            break;

        case 3:
            // Donkey Kong climb to further
            donkey_kong->base.y -= DK_CLIMB_SPEED * dt_seconds;

            if (donkey_kong->base.y <= C2_DK_FINAL_POS_Y) {
                set_animation(&donkey_kong->base, ANIM_HIDE);
                set_animation(&game_state->level.peach.base, ANIM_HIDE);
                next_cutscene_step(game_state);
            }
            break;

        case 4:
            // End of cutscene - switch back to game mode
            if (game_state->cutscene_time > 1.0f) {
                cutscene_finish(game_state);
            }
            break;
    }
}

void cutscene_dk_death(game_state_t *game_state, float dt_seconds) {
    donkey_kong_t *donkey_kong = &game_state->level.donkey_kong;
    player_t *player = &game_state->level.player;

    game_state->cutscene_time += dt_seconds;

    update_animation_progress(&donkey_kong->base, dt_seconds);

    switch (game_state->cutscene_step) {
        case 0:
            // set player pos to last position
            player->base.x = game_state->player_clone.base.x;
            player->base.y = game_state->player_clone.base.y;
            player->base.direction = game_state->player_clone.base.direction;
            if (game_state->cutscene_time > 1.0f) {
                set_animation(&donkey_kong->base, ANIM_BEATING_CHEST_DONKEY_KONG);
                next_cutscene_step(game_state);
            }
            break;
        
        case 1:
            // set dk anim falling
            if (game_state->cutscene_time > 1.6f) {
                set_animation(&donkey_kong->base, ANIM_FALLING_DONKEY_KONG);
                next_cutscene_step(game_state);
            }
            break;
        
        case 2:
            // Donkey Kong falling
            donkey_kong->base.y += C3_DK_FALLING_SPEED * dt_seconds;

            if (donkey_kong->base.y >= C3_DK_FALLING_STOP - donkey_kong->base.height) {
                peach_t *peach = &game_state->level.peach;
                peach->base.x -= C3_PEACH_OFFSET_X;
                // Set Player Pos
                player->base.x = peach->base.x + C3_MARIO_OFFSET_X;
                player->base.y = peach->base.y + peach->base.height - player->base.height;
                if (player->base.direction > 0) {
                    player->base.direction = -1;
                }
                new_effect(&game_state->level, ANIM_HEART_FULL, peach->base.x + (C3_MARIO_OFFSET_X / 2), peach->base.y - C3_HEART_OFFSET_X, 1, false);
                set_animation(&donkey_kong->base, ANIM_IDLE_FALLING_DONKEY_KONG);
                next_cutscene_step(game_state);
            }
            break;
        
        case 3:
            // End of cutscene - switch back to game mode
            if (game_state->cutscene_time > 3.0f) {
                game_state->mode = GAME_MODE_GAME_FINISH;
            }
            break;
    }
}

void cutscene_mario_death(game_state_t *game_state, float dt_seconds) {
    player_t *player = &game_state->level.player;

    game_state->cutscene_time += dt_seconds;
    update_animation_progress(&player->base, dt_seconds);

    switch (game_state->cutscene_step) {
        case 0:
            // set player death animation
            animation_sequence_t anim = get_animation_by_key(&player->base, ANIM_DEATH_MARIO);
            set_animation(&player->base, ANIM_DEATH_MARIO);

            // if animation done
            if (player->base.animation.current_frame_index == anim.frame_count - 1 &&
                player->base.animation.frame_time >= anim.frame_duration - (dt_seconds * EPSILON_2)) {
                    enemy_destroy_all(game_state);
                    next_cutscene_step(game_state);
                }
            break;

        case 1:
            // Reset Player position
            set_animation(&game_state->level.donkey_kong.base, ANIM_IDLE_DONKEY_KONG);
            set_animation(&player->base, ANIM_IDLE_MARIO);
            player->base.direction = 1;
            player->base.x = player->spawn_x;
            player->base.y = player->spawn_y; 
            next_cutscene_step(game_state);
            break;
        
        case 2:
            // End of cutscene - switch back to game mode
            if (game_state->cutscene_time > 1.0f) {
                game_state->player_lives -= 1;
                player->is_dead = false;
                player->has_hammer = false;

                game_state->cutscene_time = 0;
                game_state->cutscene_step = 0;
                game_state->mode = GAME_MODE_NORMAL;
            }
            break;
    }
}