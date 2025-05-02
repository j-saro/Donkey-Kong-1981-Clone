#include "core/sprite/sprite.h"
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
int get_type_by_name(const char *name);
animation_sequence_t get_animation_by_key(animation_state_t anim_state);
cairo_surface_t **get_animation_frames(animation_state_t anim_state);
void set_animation_frames(entity_t *base);
cairo_surface_t *get_spritesheet(entities_t entity);

// Stores the animation frames (animation -> frame)
animation_frames_t *loaded_animations = NULL;

// Stores the sprite sheets (entity -> sprite sheet)
spritesheet_t *sprite_sheets = NULL;
unsigned int num_sprite_sheets = 0;

animation_sequence_t *animations = NULL;
unsigned int num_animations = 0;

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

    for (int i = 0; i < num_sprite_sheets; i++) {
        cJSON *item = cJSON_GetArrayItem(json, i);
        if (item) {
            const char *id_str = cJSON_GetObjectItem(item, "id")->valuestring;
            sprite_sheets[i].type = get_type_by_name(id_str);

            cJSON *path_item = cJSON_GetObjectItem(item, "path");
            if (!cJSON_IsString(path_item) || strlen(path_item->valuestring) == 0) {
                g_warning("sprite_sheet[%d] missing or invalid 'path' field for '%s'", i, id_str);
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

        const char *id_str = cJSON_GetObjectItem(item, "entity_id")->valuestring;
        animations[i].type = get_type_by_name(id_str);

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

    loaded_animations = calloc(num_animations, sizeof(animation_frames_t));
    if (!loaded_animations) {
        g_warning("Failed to allocate memory for animation frame containers");
        return;
    }

    // Debug info
    //g_message("Number of loaded animations: %d", num_animations);

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

        // Create each frame
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