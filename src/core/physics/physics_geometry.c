#include "core/physics/physics_geometry.h"

void platform_collision(game_state_t *game_state);
void check_ladder_collision(game_state_t *game_state);

void platform_collision(game_state_t *game_state) {
    player_t *player = &game_state->level.player;

    float old_bottom = player->previous_y + player->base.height;
    float new_bottom = player->base.y + player->base.height;
    float player_left = player->base.x;
    float player_right = player->base.x + player->base.width;

    player->base.height = player->has_hammer ? 27 * SCALE : TILE_SIZE * SCALE;

    bool grounded = false;
    for (int i = 0; i < game_state->level.num_platforms; i++) {
        const geometry_t *platform = &game_state->level.platforms[i];

        if (!platform->has_physics)
            continue;
            
        float platform_left = platform->base.x;
        float platform_right = platform_left + platform->base.width;
        float platform_top = platform->base.y;

        // calc x-overlap
        if (!(player_right > platform_left && player_left < platform_right))
            continue;

        // snap player to platform
        if (player->base.velocity_y > 0 &&
            (old_bottom - 4) <= platform_top &&
            new_bottom >= platform_top)
        {
            player->base.y = platform_top - player->base.height;
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
    float player_center = player->base.x + player->base.width * 0.5f;
    float player_top = player->base.y;
    float player_bottom = player->base.y + player->base.height;

    if (player->has_hammer) {
        player->on_ladder = false;
        return;
    }

    bool is_on_ladder = false;

    for (int i = 0; i < game_state->level.num_ladders; i++) {
        const geometry_t *ladder = &game_state->level.ladders[i];

        if (!ladder->has_physics || player->base.animation.current_animation == ANIM_JUMP_MARIO) {
            continue;
        }

        float ladder_left = ladder->base.x;
        float ladder_right = ladder_left + ladder->base.width;
        float ladder_top = ladder->base.y - PHYSICS_EPSILON;
        float ladder_bottom = ladder_top + ladder->base.height;

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
