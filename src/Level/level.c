#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"
#include "level.h"


gboolean level_init(level_t *level);
gboolean level_load_from_json(level_t *level, const char *filename);
gboolean level_parse_from_json(level_t *level, const char *json_str);
void level_cleanup(level_t *level);
void level_draw(cairo_t *cr, const level_t *level);

gboolean level_init(level_t *level) {
    const char *filename = "data/level_data.json";
    return level_load_from_json(level, filename);
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

    level->num_platforms = cJSON_GetArraySize(platforms_json);
    level->platforms = malloc(level->num_platforms * sizeof(platform_t));

    for (int i = 0; i < level->num_platforms; i++) {
        cJSON *platform_json = cJSON_GetArrayItem(platforms_json, i);

        // Extract platform attributes (x, y, width, height)
        level->platforms[i].position.x = (float)cJSON_GetObjectItem(platform_json, "x")->valuedouble;
        level->platforms[i].position.y = (float)cJSON_GetObjectItem(platform_json, "y")->valuedouble;
        level->platforms[i].width = (float)cJSON_GetObjectItem(platform_json, "width")->valuedouble;
        level->platforms[i].height = (float)cJSON_GetObjectItem(platform_json, "height")->valuedouble;
    }

    cJSON_Delete(json);
    return TRUE;
}

void level_cleanup(level_t *level) {
    free(level->platforms);
    level->platforms = NULL;
    level->num_platforms = 0;
}

void level_draw(cairo_t *cr, const level_t *level) {
    cairo_surface_t *surface = level->sprite_sheet;
    
    for (int i = 0; i < level->num_platforms; ++i) {
        const platform_t *platform = &level->platforms[i];

        cairo_save(cr);

        cairo_pattern_t *pattern = cairo_pattern_create_for_surface(surface);
        cairo_pattern_set_extend(pattern, CAIRO_EXTEND_REPEAT);

        cairo_matrix_t matrix;
        cairo_matrix_init_translate(&matrix, -platform->position.x, -platform->position.y);
        cairo_pattern_set_matrix(pattern, &matrix);

        cairo_set_source(cr, pattern);
        cairo_rectangle(cr, 
                        platform->position.x, 
                        platform->position.y,
                        platform->width, 
                        platform->height);
        cairo_fill(cr);

        cairo_pattern_destroy(pattern);
        cairo_restore(cr);
    }
}