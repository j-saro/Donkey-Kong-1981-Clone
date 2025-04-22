#include "donkey_kong.h"
#include "movable_entity.h"
#include "animation.h"


void donkey_kong_init(donkey_kong_t *donkey_kong);
void donkey_kong_load_sprites(movable_entity_t *base);
void donkey_kong_cleanup(movable_entity_t *base);
void donkey_kong_draw(cairo_t *cr, movable_entity_t *base);
void donkey_kong_update(donkey_kong_t *donkey_kong, float dt_seconds);

void donkey_kong_init(donkey_kong_t *donkey_kong) {
    movable_entity_t *base = &donkey_kong->base;
    // Peach Position
    base->x = 73;
    base->y = 131;
    base->velocity_x = 0;
    base->velocity_y = 0;
    base->direction = 1;
    base->is_grounded = true;

    // Peach Animation
    base->animation.current_animation = ANIM_THROWING_BARREL_DONKEY_KONG;
    base->animation.current_frame_index = 0;
    base->animation.frame_time = 0;
    base->animation.frame_width = 48;
    base->animation.frame_height = 32;

    // Initialize surfaces to NULL
    base->animation.sprite_sheet = NULL;
    base->animation.current_frame = NULL;

    donkey_kong_load_sprites(&donkey_kong->base);
}

void donkey_kong_load_sprites(movable_entity_t *base) {
    const char *spritesheet = "./assets/donkey_kong_sprite_sheet.png";
    movable_entitiy_load_sprites(base, spritesheet);
}

void donkey_kong_cleanup(movable_entity_t *base) {
    movable_entitiy_cleanup(base);
}

void donkey_kong_draw(cairo_t *cr, movable_entity_t *base) {
    movable_entitiy_draw(cr, base);
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