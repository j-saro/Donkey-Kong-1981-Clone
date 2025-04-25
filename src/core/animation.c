#include "core/animation.h"
#include "core/sprite.h"

void update_animation_progress(movable_entity_t *base, float dt_seconds);
void update_animation_frame(movable_entity_t *base);
void set_animation(movable_entity_t *base, animation_state_t new_anim);


// keeps the animation frame_time updated
void update_animation_progress(movable_entity_t *base, float dt_seconds) {
    animation_t *animation = &base->animation;
    animation_sequence_t anim = animations[animation->current_animation];
    
    animation->frame_time += dt_seconds;
    
    if (animation->frame_time >= anim.frame_duration) {
        animation->frame_time = 0;

        int new_index = (animation->current_frame_index + 1) % anim.frame_count;
    
        if (new_index != animation->current_frame_index) {
            animation->current_frame_index = new_index;
            update_animation_frame(base);
        }
    }
}

void update_animation_frame(movable_entity_t *base) {
    base->animation.current_frame = animation_frame_sprites[base->type][base->animation.current_animation][base->animation.current_frame_index];
}

// sets a new Animation
void set_animation(movable_entity_t *base, animation_state_t new_anim) {
    if (base->animation.current_animation != new_anim) {
        base->animation.current_animation = new_anim;
        base->animation.current_frame_index = 0;
        base->animation.frame_time = 0;
        update_animation_frame(base);
    }
}