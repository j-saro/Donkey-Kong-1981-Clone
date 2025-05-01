#include "core/physics/physics.h"
#include "entities/abstract/item.h"
#include "core/physics/physics_enemy.h"
#include "core/physics/physics_geometry.h"
#include "core/physics/physics_utils.h"
#include "core/sprite_utils.h"

void apply_physics(game_state_t *game_state, float dt_seconds, float screen_height);
void player_gravity(player_t *player, float dt_seconds);
void window_collision(game_state_t *game_state, float screen_height);
void item_player_collision(level_t *level);

void apply_physics(game_state_t *game_state, float dt_seconds, float screen_height) {
    player_gravity(&game_state->level.player, dt_seconds);
    window_collision(game_state, screen_height);
    platform_collision(game_state);
    enemy_physics(&game_state->level, dt_seconds);
    item_player_collision(&game_state->level);
}

void player_gravity(player_t *player, float dt_seconds) {
    if (!player->climbing) {
        player->base.velocity_y += GRAVITY * dt_seconds;
        player->base.y += player->base.velocity_y * dt_seconds;
    }
}

void window_collision(game_state_t *game_state, float screen_height) {
    player_t *player = &game_state->level.player;

    float player_bottom = player->base.y + player->base.height;

    // Left/Right window collison
    if (player->base.x < 0) {
        player->base.x = 0;
    }
    if (player->base.x > BASE_WIDTH - player->base.width) {
        player->base.x = BASE_WIDTH - player->base.width;
    }
    
    // Bottom window collision
    if (player_bottom > screen_height) {
        player->base.y = screen_height - player->base.height;
        player->base.velocity_y = 0;
        player->base.is_grounded = true;
    } else {
        player->base.is_grounded = false;
    }
}

void item_player_collision(level_t *level) {
    for (int i = 0; i < level->num_items; i++) {
        item_t *item = &level->items[i];

        if (player_object_collision(&level->player, &item->base, level->player.base.width, level->player.base.height / 2.0f)) {
            item_destroy(level, i);
            level->player.has_hammer = true;
            level->player.hammer_time = 8.0f;
            i--;
            continue;
        }
    }
}