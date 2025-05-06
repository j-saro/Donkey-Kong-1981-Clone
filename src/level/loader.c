#include "level/loader.h"
#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"

#include "core/sprite/sprite.h"
#include "entities/environment/platform.h"
#include "entities/environment/ladder.h"
#include "entities/abstract/static_entity.h"
#include "entities/abstract/effect.h"
#include "entities/characters/peach.h"
#include "entities/characters/donkey_kong.h"
#include "entities/characters/player.h"
#include "entities/abstract/item.h"
#include "entities/abstract/enemy.h"

char *load_file_content_from_json(const char *filename);

// Load Sprites/Animations
gboolean sprite_load_from_json(const char *filename);
gboolean sprite_parse_from_json(const char *json_str);

// Load current level
gboolean level_load_from_json(level_t *level, const char *filename);
gboolean level_parse_from_json(level_t *level, const char *json_str);


char *load_file_content_from_json(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return NULL;

    // get file size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // allocate memory to store json file
    char *content = malloc(size + 1); // +1 for null terminator
    if (!content) {
        printf("Error allocating memory for file content\n");
        fclose(file);
        return NULL;
    }

     // Read the file content into the string
    size_t bytes_read = fread(content, 1, size, file);
    if (bytes_read != size) {
        printf("Error reading file: expected %zu bytes, but read %zu bytes\n", size, bytes_read);
        free(content);
        fclose(file);
        return NULL;
    }

    content[size] = '\0';
    fclose(file);
    return content;
}

gboolean sprite_load_from_json(const char *filename) {
    char *json_str = load_file_content_from_json(filename);
    if (!json_str) {
        printf("Failed to read sprite/animation file: %s\n", filename);
        return FALSE;
    }
    gboolean success = sprite_parse_from_json(json_str);
    free(json_str);
    return success;
}

gboolean sprite_parse_from_json(const char *json_str) {
    cJSON *json = cJSON_Parse(json_str);
    if (json == NULL) {
        printf("Error parsing JSON\n");
        return FALSE;
    }

    cJSON *animation_json = cJSON_GetObjectItem(json, "animations");
    cJSON *spritesheet_json = cJSON_GetObjectItem(json, "sprite_sheet");

    if (!cJSON_IsArray(spritesheet_json) || !cJSON_IsArray(animation_json)) {
        printf("Sprite or animation arrays missing\n");
        cJSON_Delete(json);
        return FALSE;
    }

    sprite_init(spritesheet_json, animation_json);

    cJSON_Delete(json);
    return TRUE;
}

gboolean level_load_from_json(level_t *level, const char *filename) {
    char *json_str = load_file_content_from_json(filename);
    if (!json_str) {
        printf("Failed to read level file: %s\n", filename);
        return FALSE;
    }
    gboolean success = level_parse_from_json(level, json_str);
    free(json_str);
    return success;
}

gboolean level_parse_from_json(level_t *level, const char *json_str) {
    cJSON *json = cJSON_Parse(json_str);
    if (json == NULL) {
        printf("Error parsing JSON\n");
        return FALSE;
    }

    cJSON *level_json = cJSON_GetObjectItem(json, "level");
    if (level_json == NULL) {
        printf("Error: No 'level' object found in JSON\n");
        cJSON_Delete(json);
        return FALSE;
    }

    cJSON *platforms_json = cJSON_GetObjectItem(level_json, "platforms");
    if (platforms_json == NULL || !cJSON_IsArray(platforms_json)) {
        printf("Error: No 'platforms' array found in JSON\n");
        cJSON_Delete(json);
        return FALSE;
    }

    cJSON *ladders_json = cJSON_GetObjectItem(level_json, "ladders");
    if (ladders_json == NULL || !cJSON_IsArray(ladders_json)) {
        printf("Error: No 'ladders' array found in JSON\n");
        cJSON_Delete(json);
        return FALSE;
    }

    cJSON *static_entities_json = cJSON_GetObjectItem(level_json, "static_entities");
    if (static_entities_json == NULL || !cJSON_IsArray(static_entities_json)) {
        printf("Error: No 'static_entities' array found in JSON\n");
        cJSON_Delete(json);
        return FALSE;
    }

    cJSON *peach_json = cJSON_GetObjectItem(level_json, "peach");
    if (!cJSON_IsObject(peach_json)) {
        printf("Error: 'peach' is not an object\n");
        return FALSE;
    }

    cJSON *donkey_kong_json = cJSON_GetObjectItem(level_json, "donkey_kong");
    if (!cJSON_IsObject(donkey_kong_json)) {
        printf("Error: 'donkey_kong' is not an object\n");
        return FALSE;
    }

    cJSON *mario_json = cJSON_GetObjectItem(level_json, "mario");
    if (!cJSON_IsObject(mario_json)) {
        printf("Error: 'mario' is not an object\n");
        return FALSE;
    }

    cJSON *enemy_json = cJSON_GetObjectItem(level_json, "enemys");
    if (enemy_json == NULL || !cJSON_IsArray(enemy_json)) {
        printf("Error: No 'enemys' array found in JSON\n");
        cJSON_Delete(json);
        return FALSE;
    }

    cJSON *items_json = cJSON_GetObjectItem(level_json, "items");
    if (items_json == NULL || !cJSON_IsArray(items_json)) {
        printf("Error: No 'items' array found in JSON\n");
        cJSON_Delete(json);
        return FALSE;
    }

    // Geometry
    platform_init(level, platforms_json);
    ladder_init(level, ladders_json);

    // Static entity
    static_entity_init(level, static_entities_json);

    // Movable entity
    peach_init(&level->peach, peach_json);
    donkey_kong_init(&level->donkey_kong, donkey_kong_json);
    player_init(&level->player, mario_json);

    // Items
    item_init(level, items_json);

    // Enemys
    enemy_init(level, enemy_json);

    // Effects
    effect_init(level);
    
    cJSON_Delete(json);
    return TRUE;
}