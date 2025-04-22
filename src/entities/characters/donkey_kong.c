#include "entities/characters/donkey_kong.h"
#include "entities/abstract/movable_entity.h"
#include "core/animation.h"


void donkey_kong_init(donkey_kong_t *donkey_kong, cJSON *json);
void donkey_kong_load_sprites(movable_entity_t *base);
void donkey_kong_cleanup(movable_entity_t *base);
void donkey_kong_draw(cairo_t *cr, const movable_entity_t *base);
void donkey_kong_update(donkey_kong_t *donkey_kong, float dt_seconds);

void donkey_kong_init(donkey_kong_t *donkey_kong, cJSON *json) {
    // Load Values form Json
    movable_entity_parse(&donkey_kong->base, json);

    // Donkey Kong Default Values
    donkey_kong->base.animation.current_animation = ANIM_THROWING_BARREL_DONKEY_KONG;

    donkey_kong_load_sprites(&donkey_kong->base);
}

void donkey_kong_load_sprites(movable_entity_t *base) {
    const char *spritesheet = "./assets/donkey_kong_sprite_sheet.png";
    movable_entity_load_sprites(base, spritesheet);
}

void donkey_kong_cleanup(movable_entity_t *base) {
    movable_entity_cleanup(base);
}

void donkey_kong_draw(cairo_t *cr, const movable_entity_t *base) {
    movable_entity_draw(cr, base);
}

void donkey_kong_update(donkey_kong_t *donkey_kong, float dt_seconds) {
    animation_t *animation = &donkey_kong->base.animation;
    
    update_animation_progress(animation, dt_seconds);
    animation_sequence_t sequence = animations[animation->current_animation];

    switch (animation->current_animation) {
        case ANIM_THROWING_BARREL_DONKEY_KONG:
            if (animation->current_frame_index == sequence.frame_count - 1 && 
                animation->frame_time >= (sequence.frame_duration - 0.1)) {
                animation_state_t new_anim = (rand() % 10 < 7) ? ANIM_IDLE_DONKEY_KONG : ANIM_BEATING_CHEST_DONKEY_KONG;
                set_animation(animation, new_anim);
            }
            break;
            
        case ANIM_IDLE_DONKEY_KONG:
        case ANIM_BEATING_CHEST_DONKEY_KONG:
            if (animation->current_frame_index == sequence.frame_count - 1 && 
                animation->frame_time >= (sequence.frame_duration - 0.1)) {
                set_animation(animation, ANIM_THROWING_BARREL_DONKEY_KONG);
            }
            break;
            
        default:
            set_animation(animation, ANIM_THROWING_BARREL_DONKEY_KONG);
            break;
    }
}