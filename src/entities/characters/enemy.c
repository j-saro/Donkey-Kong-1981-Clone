#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "consts.h"
#include "entities/characters/enemy.h"
#include "entities/abstract/entity.h"
#include "entities/abstract/entity_utils.h"
#include "core/sprite/animation.h"
#include "core/sprite/sprite_utils.h"

void enemy_init(level_t *level, cJSON *json);
void new_enemy(level_t *level, enemy_spawn_t *spawn);
void enemy_destroy(level_t *level, int index);
void enemy_destroy_all(game_state_t *game_state);
void enemy_cleanup(level_t *level);
void enemy_draw(cairo_t *cr, const level_t *level);
void enemy_update(level_t *level, float dt_seconds);


void enemy_init(level_t *level, cJSON *json) {
    // Spawned Enemies
    level->num_enemies = 0;
    level->enemy_capacity = 0;
    level->enemies = NULL;

    // Enemy Categories
    level->num_enemy_spawns = cJSON_GetArraySize(json);
    if (level->num_enemy_spawns > 0) {
        level->enemy_spawns = calloc(level->num_enemy_spawns, sizeof(enemy_spawn_t));
    } else {
        level->enemy_spawns = NULL;
    }
    
    for (int i = 0; i < level->num_enemy_spawns; i++) {
        cJSON *item = cJSON_GetArrayItem(json, i);
        enemy_spawn_t *spawn = &level->enemy_spawns[i];
        
        spawn->type = ENEMY;

        const char *key_str = cJSON_GetObjectItem(item, "key")->valuestring;
        animation_state_t anim_state = get_type_by_name(key_str);
        if (anim_state == -1) {
            g_warning("Unknown animation state: %s", key_str);
        }
        spawn->anim_state = anim_state;

        spawn->x = cJSON_GetObjectItem(item, "x")->valuedouble;
        spawn->y = cJSON_GetObjectItem(item, "y")->valuedouble;

        spawn->width = cJSON_GetObjectItem(item, "width")->valueint * SCALE;
        spawn->height = cJSON_GetObjectItem(item, "height")->valueint * SCALE;

        cJSON *dir_item = cJSON_GetObjectItem(item, "direction");
        spawn->direction = (dir_item && cJSON_IsNumber(dir_item)) ? dir_item->valueint : 1;      

        spawn->spawn_interval = cJSON_GetObjectItem(item, "spawn_interval")->valuedouble;
        spawn->spawn_timer = cJSON_GetObjectItem(item, "initial_delay")->valuedouble;
    }
}

void new_enemy(level_t *level, enemy_spawn_t *spawn) {
    if (!allocate_new_entity((void**)&level->enemies, level->num_enemies, &level->enemy_capacity, sizeof(enemy_t))) {
        return;
    }

    level->num_enemies += 1;
    enemy_t *enemy = &level->enemies[level->num_enemies - 1];

    init_new_entity_base(&enemy->base, spawn->x, spawn->y, spawn->direction);
    enemy->fly_time = 0;
    enemy->jumping = false;
    enemy->on_ladder = false;
    enemy->base.type = spawn->type;
    enemy->base.width = spawn->width;
    enemy->base.height = spawn->height;
    enemy->climb_direction = 1;
    enemy->base.animation.current_animation = spawn->anim_state;
    enemy->stop_x_threshold = (rand() % 2 == 0) ? ENEMY_X_THRESHOLD_1 : ENEMY_X_THRESHOLD_2;

    set_animation_frames(&enemy->base);
}

void enemy_destroy(level_t *level, int index) {
    destroy_entity(level->enemies, &level->num_enemies, sizeof(enemy_t), index);
}

void enemy_destroy_all(game_state_t *game_state) {
    // delete all enemys
    for (int i = 0; i < game_state->level.num_enemies; i++) {
        enemy_destroy(&game_state->level, i);
        i--;

    }
    // reset spawn timer
    for (int i = 0; i < game_state->level.num_enemy_spawns; i++) {
        enemy_spawn_t *spawn = &game_state->level.enemy_spawns[i];
        spawn->spawn_timer = 0.5f;
    }
}

void enemy_cleanup(level_t *level) {
    // Spawned enemies
    entity_array_cleanup((void**)&level->enemies, &level->num_enemies);
    level->enemy_capacity = 0;

    // enemy categories
    entity_array_cleanup((void**)&level->enemy_spawns, &level->num_enemy_spawns);
}

void enemy_draw(cairo_t *cr, const level_t *level) {
    for (int i = 0; i < level->num_enemies; i++) { 
        entity_draw(cr, &level->enemies[i].base);
    }
}

void enemy_update(level_t *level, float dt_seconds) {
    // spawn new enemies
    for (int i = 0; i < level->num_enemy_spawns; i++) {
        enemy_spawn_t *spawn = &level->enemy_spawns[i];

        // spawn only a max size of enemys
        if (level->num_enemies >= MAX_ENEMIES) continue;
        if (spawn->anim_state == ANIM_FIRE_SPIRIT_WALK && level->num_enemies > MAX_FIRE_SPIRITS) continue;
        if (spawn->anim_state == ANIM_FIRE_GHOST_WALK && level->num_enemies > MAX_FIRE_GHOSTS) continue;

        // spawn barrel if dk throws
        if (spawn->anim_state == ANIM_BARREL_SIDE) {
            if (level->donkey_kong.throw) {
                new_enemy(level, spawn);
                level->donkey_kong.throw = false;
            }
            continue;
        }

        spawn->spawn_timer -= dt_seconds;
    
        // spawn new enemy after timer
        if (spawn->spawn_timer <= dt_seconds) {
            new_enemy(level, spawn);
            spawn->spawn_timer = spawn->spawn_interval;
        }
    }

    // update spawned enemies
    for (int i = 0; i < level->num_enemies; i++) {
        enemy_t *enemy = &level->enemies[i];
        update_animation_progress(&enemy->base, dt_seconds);
    }
}
