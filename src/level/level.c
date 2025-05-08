#include "consts.h"
#include "level/level.h"
#include "entities/environment/platform.h"
#include "entities/environment/ladder.h"
#include "entities/abstract/static_entity.h"
#include "entities/abstract/effect.h"
#include "entities/characters/peach.h"
#include "entities/characters/donkey_kong.h"
#include "entities/characters/player.h"
#include "entities/abstract/item.h"
#include "entities/abstract/enemy.h"
#include "core/sprite/animation.h"
#include "core/physics/physics.h"
#include "level/loader.h"

gboolean level_init(game_state_t *game_state);
gboolean level_next(game_state_t *game_state);

void level_cleanup(level_t *level);
void level_draw(cairo_t *cr, game_state_t *game_state);
void level_update(game_state_t *game_state, float dt_seconds);
void level_complete(game_state_t *game_state);

// load current level
gboolean level_init(game_state_t *game_state) {
    char filename[64];
    // create current level file path
    snprintf(filename, sizeof(filename), "data/level_%d.json", game_state->current_level);

    return level_load_from_json(&game_state->level, filename);
}

gboolean level_next(game_state_t *game_state) {
    // cleanup last level
    level_cleanup(&game_state->level);
    game_state->current_level++;
    return level_init(game_state);
}

void level_cleanup(level_t *level) {
    // Geometry
    platform_cleanup(level);
    ladder_cleanup(level);

    // Static entity
    static_entity_cleanup(level);

    // enemys
    enemy_cleanup(level);

    // items
    item_cleanup(level);

    // effects
    effect_cleanup(level);
}

void level_draw(cairo_t *cr, game_state_t *game_state) {
    level_t *level = &game_state->level;
    // Geometry
    platform_draw(cr, game_state);
    ladder_draw(cr, game_state);

    // NPCs
    peach_draw(cr, &level->peach.base);
    donkey_kong_draw(cr, &level->donkey_kong.base);

    // Static entity
    static_entity_draw(cr, level);

    // Items
    item_draw(cr, level);

    // Player
    player_draw(cr, &level->player.base);

    // Enemy
    enemy_draw(cr, level);

    // Effects
    effect_draw(cr, level);
}

// Update current level
void level_update(game_state_t *game_state, float dt_seconds) {
    level_complete(game_state);

    check_ladder_collision(game_state);
    player_update(game_state, dt_seconds);
    apply_physics(game_state, dt_seconds, BASE_HEIGHT);

    peach_update(&game_state->level.peach, dt_seconds);
    donkey_kong_update(game_state, dt_seconds);
    enemy_update(&game_state->level, dt_seconds);

    // Check if player is dead
    player_check_death(&game_state->level.player);
}

// Checks if level is finished
void level_complete(game_state_t *game_state) {
    player_t *player = &game_state->level.player;
    peach_t *peach = &game_state->level.peach;
    float player_bottom = player->base.y + player->base.height;
    float peach_bottom = peach->base.y + peach->base.height;

    // if player is on the same height of peach
    if (player_bottom < peach_bottom && game_state->mode == GAME_MODE_NORMAL) {
        // Set animations
        set_animation(&game_state->level.donkey_kong.base, ANIM_IDLE_DONKEY_KONG);
        set_animation(&peach->base, ANIM_IDLE_PEACH);

        // delete all enemys
        for (int i = 0; i < game_state->level.num_enemies; i++) {
            enemy_destroy(&game_state->level, i);
            i--;
        }

        hide_static_entity(&game_state->level);
        effect_clear_all(&game_state->level);

        // faces peach and mario if not
        player->base.direction = -1;

        // Set next cutscene
        game_state->mode = GAME_MODE_CUTSCENE;
        game_state->current_cutscene = 2;
        game_state->cutscene_time = 0;
        game_state->cutscene_step = 0;
    }
}