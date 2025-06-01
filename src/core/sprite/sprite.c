#include "core/sprite/sprite.h"
#include "core/sprite/sprite_utils.h"
#include "core/sprite/animation.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

void sprite_init(cJSON *sprite_sheet_json, cJSON *animation_json);
void sprite_cleanup();
void load_sprite_sheets_from_json(cJSON *json);
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


void sprite_init(cJSON *sprite_sheet_json, cJSON *animation_json) {
    animation_load_form_json(animation_json);
    load_sprite_sheets_from_json(sprite_sheet_json);
    load_animation_sprites();
}

void sprite_cleanup() {
    free_animation_frames();
    free_sprite_sheets();
    animation_cleanup();
}

void load_sprite_sheets_from_json(cJSON *json) {
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
            const char *type_str = cJSON_GetObjectItem(item, "type")->valuestring;
            sprite_sheets[i].type = get_type_by_name(type_str);

            cJSON *path_item = cJSON_GetObjectItem(item, "path");
            if (!cJSON_IsString(path_item) || strlen(path_item->valuestring) == 0) {
                g_warning("sprite_sheet[%d] missing or invalid 'path' field for '%s'", i, type_str);
                continue;
            }

            const char *path = path_item->valuestring;
            cairo_surface_t *surface = cairo_image_surface_create_from_png(path);

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

        const char *key_str = cJSON_GetObjectItem(item, "key")->valuestring;
        animations[i].anim_key = get_type_by_name(key_str);

        const char *type_str = cJSON_GetObjectItem(item, "entity_id")->valuestring;
        animations[i].type = get_type_by_name(type_str);

        animations[i].start_x = cJSON_GetObjectItem(item, "start_x")->valueint;
        animations[i].start_y = cJSON_GetObjectItem(item, "start_y")->valueint;
        animations[i].frame_count = cJSON_GetObjectItem(item, "frame_count")->valueint;
        animations[i].frame_duration = (float)cJSON_GetObjectItem(item, "frame_duration")->valuedouble;
        animations[i].frame_width = cJSON_GetObjectItem(item, "frame_width")->valueint;
        animations[i].frame_height = (float)cJSON_GetObjectItem(item, "frame_height")->valueint;
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