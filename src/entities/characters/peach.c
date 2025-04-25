#include "entities/characters/peach.h"
#include "entities/abstract/movable_entity.h"
#include "core/animation.h"
#include "core/sprite.h"


void peach_init(peach_t *peach, cJSON *json);
void peach_draw(cairo_t *cr, const movable_entity_t *base);
void peach_update(peach_t *peach, float dt_seconds);

void peach_init(peach_t *peach, cJSON *json) {
    peach->base.type = PEACH;

    // Load Values from Json
    movable_entity_parse(&peach->base, json);

    // Peach Animation Default Values
    peach->base.animation.current_frame = NULL;
    peach->anim_interval = 3.0f;
    peach->anim_time = 0;
}


void peach_draw(cairo_t *cr, const movable_entity_t *base) {
    movable_entity_draw(cr, base);
}

void peach_update(peach_t *peach, float dt_seconds) {
    animation_t *animation = &peach->base.animation;
    animation_sequence_t sequence = animations[animation->current_animation];

    peach->anim_time += dt_seconds;

    if (animation->current_frame_index == sequence.frame_count - 1) {
        if (peach->anim_time < peach->anim_interval) {
            return;
        }

        // Restart animation
        peach->anim_time = 0;
        animation->current_frame_index = 0;
        animation->frame_time = 0;
        update_animation_frame(&peach->base);
        return;
    }

    // Normal animation progress
    update_animation_progress(&peach->base, dt_seconds);
}
