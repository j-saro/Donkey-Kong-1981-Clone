#include "level/cutscene.h"
#include "core/sprite/animation.h"
#include "core/physics/physics.h"
#include "core/physics/physics_geometry.h"
#include "entities/abstract/effect.h"

void cutscene_1(game_state_t *game_state, float dt_seconds);
void cutscene_2(game_state_t *game_state, float dt_seconds);
void cutscene_init_characters(level_t *level);

void cutscene_1(game_state_t *game_state, float dt_seconds) {
    donkey_kong_t *donkey_kong = &game_state->level.donkey_kong;
    game_state->cutscene_time += dt_seconds;

    update_animation_progress(&donkey_kong->base, dt_seconds);

    switch (game_state->cutscene_step) {
        case 0:
            // Start cutscene after 1 seconds
            if (game_state->cutscene_time >= 1.0f) {
                donkey_kong->base.x = 297;
                donkey_kong->base.y = 490;
                set_animation(&donkey_kong->base, ANIM_CLIMB_WITH_PEACH_DONKEY_KONG);
                game_state->cutscene_time = 0;
                game_state->cutscene_step++;
            }
            break;
        case 1:
            // Donkey Kong climbs ladders
            donkey_kong->base.y -= 80 * dt_seconds;

            if (donkey_kong->base.y <= 133) {
                set_animation(&donkey_kong->base, ANIM_IDLE_DONKEY_KONG);
                set_animation(&game_state->level.peach.base, ANIM_HELP_PEACH);
                game_state->cutscene_time = 0;
                game_state->cutscene_step++;
            }
            break;

        case 2:
            // Wait for 1 second
            if (game_state->cutscene_time >= 1.0f) {
                game_state->cutscene_step++;
                game_state->cutscene_time = 0;
            }
            break;

        case 3:
            // Donky Kong goes to the left
            donkey_kong->base.x -= MOVE_SPEED * dt_seconds;

            if (donkey_kong->base.x >= 130) {
                entity_jump(&donkey_kong->base, 150.0f);
            }
            apply_gravity(&donkey_kong->base, dt_seconds, 600);
            if (donkey_kong->base.y >= 133) {
                donkey_kong->base.y = 133;
                donkey_kong->base.is_grounded = true;
            }

            if (donkey_kong->base.x <= 73) {
                game_state->cutscene_time = 0;
                game_state->cutscene_step++;
            }
            break;

        case 4:
            // End of cutscene - switch back to game mode
            if (game_state->cutscene_time > 1.0f) {
                game_state->cutscene_time = 0;
                game_state->cutscene_step = 0;
                set_animation(&game_state->level.player.base, ANIM_IDLE_MARIO);
                game_state->mode = GAME_MODE_NORMAL;
            }
            break;
    }
}

void cutscene_2(game_state_t *game_state, float dt_seconds) {
    donkey_kong_t *donkey_kong = &game_state->level.donkey_kong;

    game_state->cutscene_time += dt_seconds;

    update_animation_progress(&donkey_kong->base, dt_seconds);

    switch (game_state->cutscene_step) {
        case 0:
            if (game_state->level.num_effects == 0) {
                new_effect(&game_state->level, ANIM_HEART_FULL, 295, 86, 1);
            }
            if (game_state->cutscene_time > 1.0f) {
                game_state->cutscene_time = 0;
                game_state->cutscene_step++;
            }
            break;
        case 1:
            set_animation(&donkey_kong->base, ANIM_CLIMB_WITHOUT_PEACH_DONKEY_KONG);
            donkey_kong->base.x = 178;
            donkey_kong->base.y = 131;
            game_state->cutscene_time = 0;
            game_state->cutscene_step++;
            break;
        case 2:
            // Donkey Kong climb to peach
            donkey_kong->base.y -= 80 * dt_seconds;

            if (donkey_kong->base.y <= game_state->level.peach.base.y) {
                donkey_kong->base.x = 174;
                set_animation(&donkey_kong->base, ANIM_CLIMB_WITH_PEACH_DONKEY_KONG);
                set_animation(&game_state->level.peach.base, ANIM_HIDE);
                effect_destroy(&game_state->level, 0);
                if (game_state->level.num_effects == 0) {
                    new_effect(&game_state->level, ANIM_HEART_BROKEN, 295, 86, 1);
                }
                game_state->cutscene_time = 0;
                game_state->cutscene_step++;
            }
            break;

        case 3:
            // Donkey Kong climb to further
            donkey_kong->base.y -= 80 * dt_seconds;

            if (donkey_kong->base.y <= 40) {
                set_animation(&donkey_kong->base, ANIM_HIDE);
                set_animation(&game_state->level.peach.base, ANIM_HIDE);
                game_state->cutscene_time = 0;
                game_state->cutscene_step++;
            }
            break;

        case 4:
            // End of cutscene – switch back to game mode
            if (game_state->cutscene_time > 1.0f) {
                game_state->cutscene_time = 0;
                game_state->cutscene_step = 0;
                game_state->mode = GAME_MODE_PAUSED;
            }
            break;
    }
}

void cutscene_init_characters(level_t *level) {
    set_animation(&level->peach.base, ANIM_HELP_PEACH);
    set_animation(&level->donkey_kong.base, ANIM_IDLE_DONKEY_KONG);
    set_animation(&level->player.base, ANIM_IDLE_MARIO);
}