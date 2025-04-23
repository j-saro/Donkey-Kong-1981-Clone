#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "entities/abstract/enemy.h"
#include "entities/abstract/movable_entity.h"
#include "core/animation.h"
#include "entities/characters/barrel.h"

void enemy_init(level_t *level, cJSON *json);
enemy_type_t parse_enemy_type(const char *id);
gboolean allocate_new_enemy(level_t *level);
void new_enemy(level_t *level, enemy_type_t id ,float pos_x, float pos_y, int direction);
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
    level->enemy_spawns = malloc(level->num_enemy_spawns * sizeof(enemy_spawn_t));

    for (int i = 0; i < level->num_enemy_spawns; i++) {
        cJSON *item = cJSON_GetArrayItem(json, i);
        enemy_spawn_t *spawn = &level->enemy_spawns[i];
        
        const char *id_str = cJSON_GetObjectItem(item, "id")->valuestring;
        spawn->type = parse_enemy_type(id_str);
        spawn->x = cJSON_GetObjectItem(item, "x")->valuedouble;
        spawn->y = cJSON_GetObjectItem(item, "y")->valuedouble;
        spawn->direction = cJSON_GetObjectItem(item, "direction")->valueint;
        spawn->spawn_interval = cJSON_GetObjectItem(item, "spawn_interval")->valuedouble;
        spawn->spawn_timer = cJSON_GetObjectItem(item, "initial_delay")->valuedouble;
    }
}

enemy_type_t parse_enemy_type(const char *id) {
    if (strcmp(id, "BARREL") == 0) return BARREL;
    return ERROR;
}

gboolean allocate_new_enemy(level_t *level) {
    if (level->num_enemies + 1 > level->enemy_capacity) {
        unsigned int new_capacity = level->enemy_capacity == 0 ? 4 : level->enemy_capacity + 4;

        enemy_t *tmp = realloc(level->enemies, new_capacity * sizeof(enemy_t));

        if (!tmp) {
            g_warning("Failed to allocate space for new enemy");
            return FALSE;
        }

        level->enemies = tmp;
        level->enemy_capacity = new_capacity;
    }
    return TRUE;
}

void new_enemy(level_t *level, enemy_type_t enemy_type, float pos_x, float pos_y, int direction) {
    if (!allocate_new_enemy(level)) {
        return;
    }
    level->num_enemies += 1;
    enemy_t *enemy = &level->enemies[level->num_enemies - 1];

    // Position Vaulues
    enemy->base.x = pos_x;
    enemy->base.y = pos_y;
    enemy->base.direction = direction;

    // Default Values
    enemy->base.velocity_x = 0;
    enemy->base.velocity_y = 0;
    enemy->base.is_grounded = false;

    enemy->base.animation.current_frame_index = 0;
    enemy->base.animation.frame_time = 0;
    enemy->base.animation.current_frame = NULL;

    switch (enemy_type) {
        case BARREL:
            enemy->type = enemy_type;
            enemy->base.animation.current_animation = level->barrel.animation.current_animation;
            enemy->base.animation.frame_width = level->barrel.animation.frame_width;
            enemy->base.animation.frame_height = level->barrel.animation.frame_height;
            enemy->base.animation.sprite_sheet = level->barrel.animation.sprite_sheet;
            barrel_load_sprite(enemy, level->num_enemies - 1);
            break;
        case ERROR:
            g_warning("Unknown enemy type (%d). Skipping enemy creation.", enemy_type);
    }
}

void enemy_cleanup(level_t *level) {
    for (int i = 0; i < level->num_enemies; i++) {
        level->enemies[i].base.animation.sprite_sheet = NULL; // Gets properly destroyed in barrel.c
        movable_entity_cleanup(&level->enemies[i].base.animation);
    }

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
        movable_entity_draw(cr, &level->enemies[i].base);
    }
}

void enemy_update(level_t *level, float dt_seconds) {
    for (int i = 0; i < level->num_enemy_spawns; i++) {
        enemy_spawn_t *spawn = &level->enemy_spawns[i];
        enemy_t *enemy = &level->enemies[i];

        // spawn new enemy
        spawn->spawn_timer -= dt_seconds;
        if (spawn->spawn_timer <= 0.0f) {
            new_enemy(level, spawn->type, spawn->x, spawn->y, spawn->direction);
            spawn->spawn_timer = spawn->spawn_interval;
        }

        switch (enemy->type) {
            case BARREL:
                break;
        }
        //update_animation_progress(&enemy->base.animation, dt_seconds); TODO: SEG FAULT!
    }
}
