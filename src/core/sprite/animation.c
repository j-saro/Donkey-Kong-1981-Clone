#include "core/sprite/animation.h"
#include "core/sprite/sprite_utils.h"

void update_animation_progress(entity_t *base, float dt_seconds);
void update_animation_frame(entity_t *base);
void set_animation(entity_t *base, animation_state_t new_anim);


// keeps the animation frame_time updated
void update_animation_progress(entity_t *base, float dt_seconds) {
    if (base->animation.current_animation == ANIM_HIDE) return;

    animation_t *animation = &base->animation;
    animation_sequence_t anim = get_animation_by_key(base, animation->current_animation);
    
    animation->frame_time += dt_seconds;
    
    // if current frame time is reached
    if (animation->frame_time >= anim.frame_duration) {
        animation->frame_time = 0;
        
        int new_index = (animation->current_frame_index + 1) % anim.frame_count;
    
        // if animation has more then one frame, update
        if (new_index != animation->current_frame_index) {
            animation->current_frame_index = new_index;
            update_animation_frame(base);
        }
    }
}

void update_animation_frame(entity_t *base) {
    if (base->animation.current_animation == ANIM_HIDE) return;

    // update current frame
    if (base->animation.frames) {
        base->animation.current_frame = base->animation.frames[base->animation.current_frame_index];
    }
}

// sets a new Animation
void set_animation(entity_t *base, animation_state_t new_anim) {
    if (new_anim == -1) {
        g_warning("Attempt to set invalid animation state");
        return;
    }

    // set new animation
    if (base->animation.current_animation != new_anim) {
        base->animation.current_animation = new_anim;
        base->animation.current_animation_index = -1;
        base->animation.current_frame_index = 0;
        base->animation.frame_time = 0;
        set_animation_frames(base);
        update_animation_frame(base);
    }
}