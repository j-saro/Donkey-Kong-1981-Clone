#include "entities/characters/donkey_kong.h"
#include "entities/abstract/movable_entity.h"
#include "core/animation.h"
#include "core/sprite.h"


void donkey_kong_init(donkey_kong_t *donkey_kong, cJSON *json);
void donkey_kong_draw(cairo_t *cr, const movable_entity_t *base);
void donkey_kong_update(donkey_kong_t *donkey_kong, float dt_seconds);

void donkey_kong_init(donkey_kong_t *donkey_kong, cJSON *json) {
    donkey_kong->base.type = DONKEY_KONG;
    
    // Load Values form Json
    movable_entity_parse(&donkey_kong->base, json);

    // Donkey Kong Default Values
    donkey_kong->base.animation.current_frame = NULL;
    donkey_kong->throw = false;
    donkey_kong->has_thrown_this_cicle = false;
}


void donkey_kong_draw(cairo_t *cr, const movable_entity_t *base) {
    movable_entity_draw(cr, base);
}

void donkey_kong_update(donkey_kong_t *donkey_kong, float dt_seconds) {
    animation_t *animation = &donkey_kong->base.animation;
    
    update_animation_progress(&donkey_kong->base, dt_seconds);
    animation_sequence_t sequence = animations[animation->current_animation];

    switch (animation->current_animation) {
        case ANIM_THROWING_BARREL_DONKEY_KONG:
            if (animation->current_frame_index == sequence.frame_count - 1 &&
                !donkey_kong->has_thrown_this_cicle) {
                donkey_kong->throw = true;
                donkey_kong->has_thrown_this_cicle = true;
            }
            if (animation->current_frame_index == sequence.frame_count - 1 && 
                animation->frame_time >= (sequence.frame_duration - 0.05)) {
                animation_state_t new_anim = (rand() % 10 < 7) ? ANIM_IDLE_DONKEY_KONG : ANIM_BEATING_CHEST_DONKEY_KONG;
                donkey_kong->has_thrown_this_cicle = false;
                set_animation(&donkey_kong->base, new_anim);
            }
            break;
            
        case ANIM_IDLE_DONKEY_KONG:
        case ANIM_BEATING_CHEST_DONKEY_KONG:
            if (animation->current_frame_index == sequence.frame_count - 1 && 
                animation->frame_time >= (sequence.frame_duration - 0.05)) {
                set_animation(&donkey_kong->base, ANIM_THROWING_BARREL_DONKEY_KONG);
            }
            break;
            
        default:
            set_animation(&donkey_kong->base, ANIM_THROWING_BARREL_DONKEY_KONG);
            break;
    }
}