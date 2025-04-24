#include "core/animation.h"

void animation_load_form_json(level_t *level, cJSON *json);
int get_animation_enum_by_name(const char *name);
void animation_cleanup(void);
void update_animation_progress(animation_t *animation, float dt_seconds);
void update_animation_frame(animation_t *animation);
void set_animation(animation_t *animation, animation_state_t new_anim);

animation_sequence_t *animations = NULL;

void animation_load_form_json(level_t *level, cJSON *json) {
    animations = calloc(NUM_ANIMATIONS + 1, sizeof(animation_sequence_t));

    cJSON *animation_obj;
    int i = 0;

    cJSON_ArrayForEach(animation_obj, json) {
        const char *animation_name = animation_obj->string;
        int anim_id = get_animation_enum_by_name(animation_name);
        if (anim_id == -1) {
            fprintf(stderr, "Unknown animation name: %s\n", animation_name);
            continue;
        }    

        animations[i].start_x = cJSON_GetObjectItem(animation_obj, "start_x")->valueint;
        animations[i].start_y = cJSON_GetObjectItem(animation_obj, "start_y")->valueint;
        animations[i].frame_count = cJSON_GetObjectItem(animation_obj, "frame_count")->valueint;
        animations[i].frame_duration = (float)cJSON_GetObjectItem(animation_obj, "frame_duration")->valuedouble;

        i++;
    }
}

int get_animation_enum_by_name(const char *name) {
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
    if (strcmp(name, "ANIM_BARREL_FRONT") == 0) return ANIM_BARREL_FRONT;
    return -1; 
}

void animation_cleanup(void) {
    free(animations);
    animations = NULL;
}

// keeps the animation frame_time updated
void update_animation_progress(animation_t *animation, float dt_seconds) {
    animation_sequence_t anim = animations[animation->current_animation];
    
    animation->frame_time += dt_seconds;
    
    if (animation->frame_time >= anim.frame_duration) {
        animation->frame_time = 0;

        int new_index = (animation->current_frame_index + 1) % anim.frame_count;
    
        if (new_index != animation->current_frame_index) {
            animation->current_frame_index = new_index;
            update_animation_frame(animation);
        }
    }
}

void update_animation_frame(animation_t *animation) {
    if (animation->current_frame != NULL) {
        cairo_surface_destroy(animation->current_frame);
        animation->current_frame = NULL;
    }

    animation_sequence_t sequence = animations[animation->current_animation];

    int x = sequence.start_x + (animation->current_frame_index * animation->frame_width);
    int y = sequence.start_y;

    animation->current_frame = cairo_surface_create_for_rectangle(
        animation->sprite_sheet,
        x, y,
        animation->frame_width,
        animation->frame_height
    );

    if (cairo_surface_status(animation->current_frame) != CAIRO_STATUS_SUCCESS) {
        g_warning("Failed to create animation frame surface at (%d,%d)", x, y);
    }
}

// sets a new Animation
void set_animation(animation_t *animation, animation_state_t new_anim) {
    if (animation->current_animation != new_anim) {
        animation->current_animation = new_anim;
        animation->current_frame_index = 0;
        animation->frame_time = 0;
        update_animation_frame(animation);
    }
}