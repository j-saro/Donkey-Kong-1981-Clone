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
#include "core/gui.h"
#include "level/loader.h"

void level_init(game_state_t *game_state);
gboolean level_load(game_state_t *game_state, const char *file_path, int file_index);
gboolean level_next(game_state_t *game_state, const char *file_path, int *file_index);

void level_cleanup(level_t *level);
void level_draw(cairo_t *cr, game_state_t *game_state);
void level_update(game_state_t *game_state, float dt_seconds);
void level_internal_update(game_state_t *game_state, float dt_seconds);
void level_complete(game_state_t *game_state);
void on_level_finish(game_state_t *game_state);

void level_init(game_state_t *game_state) {
    // Level
    game_state->current_level = 0;
    game_state->player_score = 0;
    game_state->bonus_live = false;
    game_state->player_lives = PLAYER_LIVES;
    game_state->pressed_buttons = NUM_BUTTONS;
    game_state->bonus_points_timer = DECREMENT_TIMER;

    // Cutscene
    game_state->current_cutscene = CUTSCENE_DK_INTRO;
    game_state->cutscene_time = 0.0f;
    game_state->cutscene_step = 0;
}

// load current level
gboolean level_load(game_state_t *game_state, const char *file_path, int file_index) {
    game_state->bonus_points = BASE_BONUS_POINTS + game_state->current_level * LEVEL_BONUS_POINTS;
    char filename[64];
    // create current level file path
    snprintf(filename, sizeof(filename), "%s%d.json", file_path, file_index);
    return level_load_from_json(&game_state->level, filename);
}

gboolean level_next(game_state_t *game_state, const char *file_path, int *file_index) {
    // cleanup last level
    level_cleanup(&game_state->level);
    (*file_index)++;
    return level_load(game_state, file_path, *file_index);
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

    // Items
    item_draw(cr, level);

    // Effects
    effect_draw(cr, level);

    // Player
    player_draw(cr, &level->player.base);

    // Static entity
    static_entity_draw(cr, level);

    // Enemy
    enemy_draw(cr, level);
}

// Update current level
void level_update(game_state_t *game_state, float dt_seconds) {
    level_internal_update(game_state, dt_seconds);
    level_complete(game_state);

    gui_update(game_state, dt_seconds);

    platform_update(&game_state->level, dt_seconds);

    check_ladder_collision(game_state);
    player_update(game_state, dt_seconds);
    apply_physics(game_state, dt_seconds, BASE_HEIGHT);

    peach_update(&game_state->level.peach, dt_seconds);
    donkey_kong_update(game_state, dt_seconds);
    static_entity_update(&game_state->level, dt_seconds);
    enemy_update(&game_state->level, dt_seconds);
    effect_update(&game_state->level, dt_seconds);

    // Check if player is dead
    player_check_death(game_state);
}

void level_internal_update(game_state_t *game_state, float dt_seconds) {
    // frame timer
    game_state->level.frame_timer += dt_seconds;
    if (game_state->level.frame_timer >= 1.0f) {
        game_state->level.frame_timer = 0.0f;
    }
    if (!game_state->bonus_live && game_state->player_score > BONUS_LIVE) {
        game_state->bonus_live = true;
        game_state->player_lives += 1;
    }
}

// Checks if level is finished
void level_complete(game_state_t *game_state) {
    player_t *player = &game_state->level.player;
    float player_bottom = player->base.y + player->base.height;

    if (game_state->pressed_buttons <= 0) {
        on_level_finish(game_state);
        game_state->player_clone = game_state->level.player;
        game_state->current_cutscene = CUTSCENE_DK_DEATH;

        // load dk death cutscene
        level_cleanup(&game_state->level);
        level_load(game_state, CUTSCENE_FILE_PATH, CUTSCENE_DK_DEATH);

        // reset level for gui
        game_state->current_level = 4;
        game_state->bonus_points = 0;
        return;
    }

    // if player is on the same height of peach
    if (player_bottom < game_state->level.finish_line && game_state->mode == GAME_MODE_NORMAL) {
        on_level_finish(game_state);

        // Set next cutscene
        game_state->current_cutscene = CUTSCENE_DK_ESCAPE;
    }
}

void on_level_finish(game_state_t *game_state) {
    peach_t *peach = &game_state->level.peach;

    // Set animations
    set_animation(&game_state->level.donkey_kong.base, ANIM_IDLE_DONKEY_KONG);
    set_animation(&peach->base, ANIM_IDLE_PEACH);

    // delete all enemys
    enemy_destroy_all(game_state);
    hide_static_entity(&game_state->level);
    effect_clear_all(&game_state->level);

    game_state->player_score += game_state->bonus_points;

    game_state->mode = GAME_MODE_CUTSCENE;
    game_state->cutscene_time = 0;
    game_state->cutscene_step = 0;
}