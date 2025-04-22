#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"
#include "level/level.h"
#include "entities/environment/platform.h"
#include "entities/environment/ladder.h"
#include "entities/abstract/static_entity.h"
#include "entities/characters/peach.h"
#include "entities/characters/donkey_kong.h"
#include "entities/characters/player.h"

gboolean level_init(game_state_t *game_state);
gboolean level_load_from_json(level_t *level, const char *filename);
gboolean level_parse_from_json(level_t *level, const char *json_str);
void level_cleanup(level_t *level);
void level_draw(cairo_t *cr, const level_t *level);

gboolean level_init(game_state_t *game_state) {
    const char *filename = "data/level_data.json";
    return level_load_from_json(&game_state->level, filename);
}

gboolean level_load_from_json(level_t *level, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        return FALSE;
    }

    // get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // allocate memory to store json file
    char *json_str = malloc(file_size + 1); // +1 for null terminator
    if (json_str == NULL) {
        printf("Error allocating memory for file content\n");
        fclose(file);
        return FALSE;
    }

    // Read the file content into the string
    size_t bytes_read = fread(json_str, 1, file_size, file);
    if (bytes_read != file_size) {
        printf("Error reading file: expected %zu bytes, but read %zu bytes\n", file_size, bytes_read);
        free(json_str);
        fclose(file);
        return FALSE;
    }
    json_str[file_size] = '\0';

    fclose(file);

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
        printf("Error: 'donkey_kong' is not an object\n");
        return FALSE;
    }

    platform_init(level, platforms_json);
    ladder_init(level, ladders_json);
    static_entity_init(level, static_entities_json);
    peach_init(&level->peach, peach_json);
    donkey_kong_init(&level->donkey_kong, donkey_kong_json);
    player_init(&level->player, mario_json);

    cJSON_Delete(json);
    return TRUE;
}

void level_cleanup(level_t *level) {
    platform_cleanup(level);
    ladder_cleanup(level);
    static_entity_cleanup(level);
    peach_cleanup(&level->peach.base);
    donkey_kong_cleanup(&level->donkey_kong.base);
    player_cleanup(&level->player.base);
}

void level_draw(cairo_t *cr, const level_t *level) {
    platform_draw(cr, level);
    ladder_draw(cr, level);
    static_entity_draw(cr, level);
    peach_draw(cr, &level->peach.base);
    donkey_kong_draw(cr, &level->donkey_kong.base);
    player_draw(cr, &level->player.base);
}