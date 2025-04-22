#include "animation.h"

const animation_sequence_t animations[] = {
    // Mario
    [ANIM_IDLE_MARIO] = {1, 21, 1, 0.2f},
    [ANIM_WALK_MARIO] = {1, 21, 3, 0.1f},
    [ANIM_JUMP_MARIO] = {63, 21, 1, 0.15f},
    [ANIM_CLIMB_MARIO] = {132, 21, 2, 0.1f},
    [ANIM_CLIMB_IDLE_MARIO] = {132, 21, 1, 1.0f},

    // Peach
    [ANIM_IDLE_PEACH] = {0, 0, 2, 0.3f},

    // Donkey Kong
    [ANIM_IDLE_DONKEY_KONG] = {1, 21, 1, 0.3f},
    [ANIM_BEATING_CHEST_DONKEY_KONG] = {54, 21, 1, 0.3f},
    [ANIM_THROWING_BARREL_DONEKY_KONG] = {156, 21, 3, 1.0f},
};

void update_animation_progress(animation_t *animation, float dt_seconds);
void update_animation_frame(animation_t *animation);
void set_animation(animation_t *animation, animation_state_t new_anim);


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