#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "entities/abstract/enemy.h"
#include "entities/abstract/entity.h"
#include "entities/abstract/entity_utils.h"
#include "core/sprite/animation.h"
#include "core/sprite/sprite_utils.h"

void enemy_init(level_t *level, cJSON *json);
void new_enemy(level_t *level, enemy_spawn_t *spawn);
void enemy_destroy(level_t *level, int index);
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
    level->enemy_spawns = calloc(level->num_enemy_spawns, sizeof(enemy_spawn_t));

    for (int i = 0; i < level->num_enemy_spawns; i++) {
        cJSON *item = cJSON_GetArrayItem(json, i);
        enemy_spawn_t *spawn = &level->enemy_spawns[i];
        
        const char *type_str = cJSON_GetObjectItem(item, "type")->valuestring;
        spawn->type = get_type_by_name(type_str);
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
    if (!allocate_new_entity((void**)&level->enemies, &level->num_enemies, &level->enemy_capacity, sizeof(enemy_t))) {
        return;
    }

    level->num_enemies += 1;
    enemy_t *enemy = &level->enemies[level->num_enemies - 1];

    init_new_entity_base(&enemy->base, spawn->x, spawn->y, spawn->direction);
    enemy->fly_time = 0;
    enemy->jumping = false;
    enemy->base.type = spawn->type;
    enemy->base.width = spawn->width;
    enemy->base.height = spawn->height;

    switch (spawn->type) {
        case BARREL:
            enemy->base.animation.current_animation = ANIM_BARREL_SIDE;
            break;
        case FIRE_SPIRIT:
            enemy->base.animation.current_animation = ANIM_FIRE_SPIRIT_WALK;
            break;

        default:
            g_warning("Unknown enemy type (%d). Skipping enemy creation.", spawn->type);
    }

    set_animation_frames(&enemy->base);
}

void enemy_destroy(level_t *level, int index) {
    destroy_entity(level->enemies, &level->num_enemies, sizeof(enemy_t), index);
}

void enemy_cleanup(level_t *level) {
    // Spawned enemies
    free(level->enemies);
    level->enemies = NULL;
    level->enemy_capacity = 0;
    level->num_enemies = 0;

    // enemy categories
    free(level->enemy_spawns);
    level->enemy_spawns = NULL;
    level->enemy_spawns = 0;
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

        if (level->num_enemies >= MAX_ENEMIES) {
            continue;
        }

        if (level->donkey_kong.throw && spawn->type == BARREL) {
            new_enemy(level, spawn);
            level->donkey_kong.throw = false;
            continue;
        }

        spawn->spawn_timer -= dt_seconds;
    
        if (spawn->spawn_timer <= dt_seconds && !(spawn->type == BARREL)) {
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
