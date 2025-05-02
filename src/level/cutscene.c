#include "level/cutscene.h"
#include "core/sprite/animation.h"

void cutscene_1(game_state_t *game_state, float dt_seconds);
void cutscene_init_characters(level_t *level);

void cutscene_1(game_state_t *game_state, float dt_seconds) {
    donkey_kong_t *donkey_kong = &game_state->level.donkey_kong;
    game_state->cutscene_time += dt_seconds;

    update_animation_progress(&donkey_kong->base, dt_seconds);

    switch (game_state->cutscene_step) {
        case 0:
            if (game_state->cutscene_time < 0.2) {
                donkey_kong->base.x = 297;
                donkey_kong->base.y = 506;
                set_animation(&donkey_kong->base, ANIM_CLIMB_DONKEY_KONG);
                break;
            }
            // Donkey Kong climbs ladders
            donkey_kong->base.y -= 75 * dt_seconds;

            if (donkey_kong->base.y <= 133) {
                set_animation(&donkey_kong->base, ANIM_BEATING_CHEST_DONKEY_KONG);
                set_animation(&game_state->level.peach.base, ANIM_HELP_PEACH);
                game_state->cutscene_time = 0;
                game_state->cutscene_step++;
            }
            break;

        case 1:
            // Donkey Kong beats chest for 2 seconds
            if (game_state->cutscene_time >= 2.0f) {
                set_animation(&donkey_kong->base, ANIM_IDLE_DONKEY_KONG);
                game_state->cutscene_step++;
                game_state->cutscene_time = 0;
            }
            break;

        case 2:
            // Donky Kong goes to the left
            donkey_kong->base.x -= MOVE_SPEED * dt_seconds;

            if (donkey_kong->base.x <= 73) {
                game_state->cutscene_time = 0;
                game_state->cutscene_step++;
            }
            break;

        case 3:
            // End of cutscene – switch back to game mode
            if (game_state->cutscene_time > 1.0f) {
                game_state->cutscene_time = 0;
                game_state->cutscene_step = 0;
                set_animation(&game_state->level.player.base, ANIM_IDLE_MARIO);
                game_state->mode = GAME_MODE_NORMAL;
            }
            break;
    }
}

void cutscene_init_characters(level_t *level) {
    set_animation(&level->peach.base, ANIM_HELP_PEACH);
    set_animation(&level->donkey_kong.base, ANIM_IDLE_DONKEY_KONG);
    set_animation(&level->player.base, ANIM_IDLE_MARIO);
}