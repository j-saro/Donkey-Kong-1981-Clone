#include "core/sprite/sprite.h"
#include "core/sprite/sprite_utils.h"
#include "core/sprite/animation.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

void sprite_init(game_state_t *game_state, cJSON *sprite_sheet_json, cJSON *animation_json);
void sprite_cleanup();
void load_sprite_sheets_from_json(game_state_t *game_state, cJSON *json);
void load_animation_sprites();
void animation_load_form_json(cJSON *json);
void free_animation_frames();
void free_sprite_sheets();
void animation_cleanup();

/* Stores the sprite sheets in json order

Struct
    entities_t type;
    cairo_surface_t *sprite_sheet;
*/
spritesheet_t *sprite_sheets = NULL;
unsigned int num_sprite_sheets = 0;


/* Stores the animation in json order

Struct
    entities_t type;
    animation_state_t anim_key;
    int start_x;
    ...
*/
animation_sequence_t *animations = NULL;
unsigned int num_animations = 0;


/* Stores the loaded animation frames in animation order 
   (Animation index == Loaded Animation index)
   frames = loaded_animations[animation_index].frames

Struct
    animation_state_t anim_key;
    cairo_surface_t **frames;
    float frame_count;
*/
animation_frames_t *loaded_animations = NULL;


void sprite_init(game_state_t *game_state, cJSON *sprite_sheet_json, cJSON *animation_json) {
    animation_load_form_json(animation_json);
    load_sprite_sheets_from_json(game_state, sprite_sheet_json);
    load_animation_sprites();
}

void sprite_cleanup() {
    free_animation_frames();
    free_sprite_sheets();
    animation_cleanup();
}

void load_sprite_sheets_from_json(game_state_t *game_state, cJSON *json) {
    num_sprite_sheets = cJSON_GetArraySize(json);
    sprite_sheets = calloc(num_sprite_sheets, sizeof(spritesheet_t));
    if (!sprite_sheets) {
        g_warning("Failed to allocate memory for sprite sheets");
        return;
    }

    // load each spritesheet and its surface into an array
    for (int i = 0; i < num_sprite_sheets; i++) {
        cJSON *item = cJSON_GetArrayItem(json, i);
        if (item) {
             
            cJSON *type_item = cJSON_GetObjectItem(item, "type");
            if (type_item && cJSON_IsString(type_item)) {
                sprite_sheets[i].type = get_type_by_name(type_item->valuestring);
            } else {
                g_warning("sprite_sheet[%d] missing or invalid 'type' field", i);
                continue;
            }
            
            const char *path = NULL;
            cJSON *path_item = cJSON_GetObjectItem(item, "path");
            if (cJSON_IsString(path_item) && strlen(path_item->valuestring) > 0) {
                path = path_item->valuestring;
            } else {
                g_warning("sprite_sheet[%d] missing or invalid 'path' field for '%s'", i, type_item->valuestring);
                continue;
            }

            // Create spritsheet path
            const char *folder_path = game_state->arguments.level_folder_path;
            int file_path_len = strlen(folder_path) + strlen(path) + 1;

            char *stylesheet_path = malloc(file_path_len);
            if (stylesheet_path) {
                snprintf(stylesheet_path, file_path_len, "%s%s", folder_path, path);
            } else {
                g_warning("Failed to allocate memory for spritsheet path");
                continue;
            }

            cairo_surface_t *surface = cairo_image_surface_create_from_png(stylesheet_path);
            free(stylesheet_path);

            if (cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS) {
                g_warning("Failed to load sprite sheet: %s", path);
                cairo_surface_destroy(surface);
                continue;
            }

            sprite_sheets[i].sprite_sheet = surface;
        }
    }
}

void animation_load_form_json(cJSON *json) {
    num_animations = cJSON_GetArraySize(json);
    animations = calloc(num_animations, sizeof(animation_sequence_t));

    for (int i = 0; i < num_animations; i++) {
        cJSON *item = cJSON_GetArrayItem(json, i);

        cJSON *key_item = cJSON_GetObjectItem(item, "key");
        cJSON *type_item = cJSON_GetObjectItem(item, "entity_id");
        cJSON *start_x_item = cJSON_GetObjectItem(item, "start_x");
        cJSON *start_y_item = cJSON_GetObjectItem(item, "start_y");
        cJSON *frame_count_item = cJSON_GetObjectItem(item, "frame_count");
        cJSON *frame_duration_item = cJSON_GetObjectItem(item, "frame_duration");
        cJSON *frame_width_item = cJSON_GetObjectItem(item, "frame_width");
        cJSON *frame_height_item = cJSON_GetObjectItem(item, "frame_height");

        if (!cJSON_IsString(key_item) || !cJSON_IsString(type_item) ||
            !cJSON_IsNumber(start_x_item) || !cJSON_IsNumber(start_y_item) ||
            !cJSON_IsNumber(frame_count_item) || !cJSON_IsNumber(frame_duration_item) ||
            !cJSON_IsNumber(frame_width_item) || !cJSON_IsNumber(frame_height_item)) {
            
            g_warning("Animation %d: invalid or missing fields", i);
            continue;
        }

        animations[i].anim_key = get_type_by_name(key_item->valuestring);
        animations[i].type = get_type_by_name(type_item->valuestring);
        animations[i].start_x = start_x_item->valueint;
        animations[i].start_y = start_y_item->valueint;
        animations[i].frame_count = frame_count_item->valueint;
        animations[i].frame_duration = (float)frame_duration_item->valuedouble;
        animations[i].frame_width = frame_width_item->valueint;
        animations[i].frame_height = (float)frame_height_item->valueint;
    }
}

void load_animation_sprites() {
    if (animations == NULL || sprite_sheets == NULL) {
        g_warning("Animations or spritesheets are not initialized.");
        return;
    }

    // calloc memory for every animation loaded
    loaded_animations = calloc(num_animations, sizeof(animation_frames_t));
    if (!loaded_animations) {
        g_warning("Failed to allocate memory for animation frame containers");
        return;
    }

    // for every animation loaded
    for (int i = 0; i < num_animations; i++) {
        animation_sequence_t *anim = &animations[i];
        loaded_animations[i].anim_key = anim->anim_key;
        loaded_animations[i].frame_count = anim->frame_count;

        // Allocate memory for the frames
        loaded_animations[i].frames = calloc(anim->frame_count, sizeof(cairo_surface_t *));
        if (!loaded_animations[i].frames) {
            g_warning("Failed to allocate memory for frames of animation %d", i);
            continue;
        }

        // Find matching sprite sheet
        cairo_surface_t *sheet = get_spritesheet(anim->type);
        if (!sheet) {
            g_warning("No matching sprite sheet found for animation type %d", anim->type);
            continue;
        }

        // Create surface for each frame
        for (int k = 0; k < anim->frame_count; k++) {
            loaded_animations[i].frames[k] = cairo_surface_create_for_rectangle(
                sheet,
                anim->start_x + k * anim->frame_width,
                anim->start_y,
                anim->frame_width,
                anim->frame_height
            );

            // Debug info
            //g_message("Animation %d (anim_key=%d), frame %d -> surface address: %p",
            //    i, anim->anim_key, k, (void*)loaded_animations[i].frames[k]);
        }
    }
}

void free_animation_frames() {
    if (!loaded_animations) return;

    for (int i = 0; i < num_animations; i++) {
        if (!loaded_animations[i].frames) continue;

        for (int k = 0; k < loaded_animations[i].frame_count; k++) {
            if (loaded_animations[i].frames[k]) {
                cairo_surface_destroy(loaded_animations[i].frames[k]);
            }
        }

        free(loaded_animations[i].frames);
        loaded_animations[i].frames = NULL;
    }

    free(loaded_animations);
    loaded_animations = NULL;
}

void free_sprite_sheets() {
    for (int i = 0; i < num_sprite_sheets; i++) {
        if (sprite_sheets[i].sprite_sheet) {
            cairo_surface_destroy(sprite_sheets[i].sprite_sheet);
        }
    }
    free(sprite_sheets);
    sprite_sheets = NULL;
}

void animation_cleanup() {
    free(animations);
    animations = NULL;
}