#include "core/sprite.h"
#include "core/animation.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void sprite_init(cJSON *sprite_sheet_json, cJSON *animation_json);
void sprite_cleanup();
void load_sprite_sheets(cJSON *json);
void load_animation_sprites();
void animation_load_form_json(cJSON *json);
void free_animation_frames();
void free_sprite_sheets();
void animation_cleanup();
int get_type_by_name(const char *name);

// Stores the animation frames (3D array: entity -> animation -> frame)
cairo_surface_t ****animation_frame_sprites = NULL;
unsigned int num_animations = 0;

// Stores the sprite sheets (2D array: entity -> sprite sheet)
cairo_surface_t **sprite_sheets = NULL;

animation_sequence_t *animations = NULL;

void sprite_init(cJSON *sprite_sheet_json, cJSON *animation_json) {
    animation_load_form_json(animation_json);
    load_sprite_sheets(sprite_sheet_json);
    load_animation_sprites();
}

void sprite_cleanup() {
    free_animation_frames();
    free_sprite_sheets();
    animation_cleanup();
}

void load_sprite_sheets(cJSON *json) {
    sprite_sheets = calloc(ENTITY_COUNT, sizeof(cairo_surface_t *));
    if (!sprite_sheets) {
        g_warning("Failed to allocate memory for sprite sheets");
        return;
    }

    for (int i = 0; i < ENTITY_COUNT; i++) {
        sprite_sheets[i] = NULL;
    }

    for (int i = 0; i < ENTITY_COUNT; i++) {
        cJSON *item = cJSON_GetArrayItem(json, i);
        if (item) {
            const char *id_str = cJSON_GetObjectItem(item, "id")->valuestring;
            entities_t id = get_type_by_name(id_str);

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

            sprite_sheets[id] = surface;
        }
    }
}

void animation_load_form_json(cJSON *json) {
    num_animations = cJSON_GetArraySize(json);
    animations = calloc(num_animations, sizeof(animation_sequence_t));

    for (int i = 0; i < num_animations; i++) {
        cJSON *item = cJSON_GetArrayItem(json, i);

        const char *key_str = cJSON_GetObjectItem(item, "key")->valuestring;
        animation_state_t id = get_type_by_name(key_str);

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
    // Ensure animations array exists and has been populated
    if (animations == NULL) {
        g_warning("Animations array is not initialized.");
        return;
    }

    // Allocate memory for the 3D array (entity -> animation -> frame)
    animation_frame_sprites = calloc(ENTITY_COUNT, sizeof(cairo_surface_t ***));
    if (!animation_frame_sprites) {
        g_warning("Failed to allocate memory for animation frames");
        return;
    }

    for (int i = 0; i < ENTITY_COUNT; i++) {
        animation_frame_sprites[i] = calloc(ANIMATION_COUNT, sizeof(cairo_surface_t **));
        if (!animation_frame_sprites[i]) {
            g_warning("Failed to allocate memory for animation frames for entity %d", i);
            return;
        }

        for (int j = 0; j < ANIMATION_COUNT; j++) {
            animation_frame_sprites[i][j] = NULL; // Initialize to NULL by default
        
            if (animations[j].type == i) {
                animation_frame_sprites[i][j] = calloc(animations[j].frame_count, sizeof(cairo_surface_t *));
                if (!animation_frame_sprites[i][j]) {
                    g_warning("Failed to allocate memory for frames of animation %d (entity %d)", j, i);
                    return;
                }
        
                // Initialize to NULL to avoid invalid free later
                for (int k = 0; k < animations[j].frame_count; k++) {
                    animation_frame_sprites[i][j][k] = NULL;
                }
            }
        }
    }

    for (int i = 0; i < ENTITY_COUNT; i++) {
        // Iterate through animations
        for (int j = 0; j < num_animations; j++) {

            // Check if the animation is for the current entity (e.g., MARIO)
            if (animations[j].type == i) {

                // Loop through each frame of the animation
                for (int k = 0; k < animations[j].frame_count; k++) {

                    // Create the frame for the animation from the sprite sheet
                    animation_frame_sprites[i][j][k] = cairo_surface_create_for_rectangle(
                        sprite_sheets[i],  // Sprite sheet for the current entity
                        animations[j].start_x + k * animations[j].frame_width,
                        animations[j].start_y,
                        animations[j].frame_width,
                        animations[j].frame_height
                    );
                }
            }
        }
    }
}

void free_animation_frames() {
    if (!animation_frame_sprites) return;

    for (int i = 0; i < ENTITY_COUNT; i++) {
        if (!animation_frame_sprites[i]) continue;

        for (int j = 0; j < ANIMATION_COUNT; j++) {
            if (!animation_frame_sprites[i][j]) continue;

            for (int k = 0; k < animations[j].frame_count; k++) {
                if (animation_frame_sprites[i][j][k]) {
                    cairo_surface_destroy(animation_frame_sprites[i][j][k]);
                }
            }

            free(animation_frame_sprites[i][j]);
            animation_frame_sprites[i][j] = NULL;
        }

        free(animation_frame_sprites[i]);
        animation_frame_sprites[i] = NULL;
    }

    free(animation_frame_sprites);
    animation_frame_sprites = NULL;
}

void free_sprite_sheets() {
    if (sprite_sheets) {
        for (int i = 0; i < ENTITY_COUNT; i++) {
            if (sprite_sheets[i]) {
                cairo_surface_destroy(sprite_sheets[i]);
            }
        }
        free(sprite_sheets);
    }
}

void animation_cleanup() {
    free(animations);
    animations = NULL;
}

int get_type_by_name(const char *name) {
    // Entities
    if (strcmp(name, "MARIO") == 0) return MARIO;
    if (strcmp(name, "DONKEY_KONG") == 0) return DONKEY_KONG;
    if (strcmp(name, "PEACH") == 0) return PEACH;
    if (strcmp(name, "BARREL") == 0) return BARREL;
    if (strcmp(name, "HAMMER") == 0) return HAMMER;
    if (strcmp(name, "LADDER") == 0) return LADDER;
    if (strcmp(name, "PLATFORM") == 0) return PLATFORM;
    if (strcmp(name, "STATIC_ENTITY") == 0) return STATIC_ENTITY;

    // Animations
    if (strcmp(name, "ANIM_IDLE_MARIO") == 0) return ANIM_IDLE_MARIO;
    if (strcmp(name, "ANIM_WALK_MARIO") == 0) return ANIM_WALK_MARIO;
    if (strcmp(name, "ANIM_JUMP_MARIO") == 0) return ANIM_JUMP_MARIO;
    if (strcmp(name, "ANIM_CLIMB_MARIO") == 0) return ANIM_CLIMB_MARIO;
    if (strcmp(name, "ANIM_CLIMB_IDLE_MARIO") == 0) return ANIM_CLIMB_IDLE_MARIO;
    if (strcmp(name, "ANIM_IDLE_PEACH") == 0) return ANIM_IDLE_PEACH;
    if (strcmp(name, "ANIM_IDLE_DONKEY_KONG") == 0) return ANIM_IDLE_DONKEY_KONG;
    if (strcmp(name, "ANIM_BEATING_CHEST_DONKEY_KONG") == 0) return ANIM_BEATING_CHEST_DONKEY_KONG;
    if (strcmp(name, "ANIM_THROWING_BARREL_DONKEY_KONG") == 0) return ANIM_THROWING_BARREL_DONKEY_KONG;
    if (strcmp(name, "ANIM_BARREL_SIDE") == 0) return ANIM_BARREL_SIDE;
    if (strcmp(name, "ANIM_BARREL_FRONT_IDLE") == 0) return ANIM_BARREL_FRONT_IDLE;
    if (strcmp(name, "ANIM_BARREL_FRONT") == 0) return ANIM_BARREL_FRONT;
    if (strcmp(name, "ANIM_OIL_BARREL") == 0) return ANIM_OIL_BARREL;
    if (strcmp(name, "ANIM_BARREL_STACK") == 0) return ANIM_BARREL_STACK;
    if (strcmp(name, "ANIM_STATIC_ENTITY_HIDE") == 0) return ANIM_STATIC_ENTITY_HIDE;

    return -1;
}