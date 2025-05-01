#include "entities/characters/peach.h"
#include "entities/abstract/entity.h"
#include "core/animation.h"
#include "core/sprite.h"
#include "core/sprite_utils.h"


void peach_init(peach_t *peach, cJSON *json);
void peach_draw(cairo_t *cr, const entity_t *base);
void peach_update(peach_t *peach, float dt_seconds);

void peach_init(peach_t *peach, cJSON *json) {
    peach->base.type = PEACH;

    // Load Values from Json
    entity_parse(&peach->base, json);

    // Peach Animation Default Values
    peach->anim_interval = 6.0f;
    peach->anim_time = 0;
}


void peach_draw(cairo_t *cr, const entity_t *base) {
    entity_draw(cr, base);
}

void peach_update(peach_t *peach, float dt_seconds) {
    animation_t *animation = &peach->base.animation;
    animation_sequence_t sequence = get_animation_by_key(&peach->base, animation->current_animation);

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
