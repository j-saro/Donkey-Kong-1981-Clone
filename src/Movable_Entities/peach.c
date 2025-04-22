#include "peach.h"
#include "movable_entity.h"
#include "animation.h"


void peach_init(peach_t *peach);
void peach_load_sprites(movable_entity_t *base);
void peach_cleanup(movable_entity_t *base);
void peach_draw(cairo_t *cr, movable_entity_t *base);
void peach_update(peach_t *peach, float dt_seconds);

void peach_init(peach_t *peach) {
    movable_entity_t *base = &peach->base;
    // Peach Position
    base->x = 250;
    base->y = 91;
    base->velocity_x = 0;
    base->velocity_y = 0;
    base->direction = 1;
    base->is_grounded = false;

    // Peach Animation
    base->animation.current_animation = ANIM_IDLE_PEACH;
    base->animation.current_frame_index = 0;
    base->animation.frame_time = 0;
    base->animation.frame_width = TILE_SIZE;
    base->animation.frame_height = 24;
    peach->anim_interval = 3.0f;
    peach->frame_time = 0;

    // Initialize surfaces to NULL
    base->animation.sprite_sheet = NULL;
    base->animation.current_frame = NULL;

    peach_load_sprites(&peach->base);
}

void peach_load_sprites(movable_entity_t *base) {
    const char *spritesheet = "./assets/peach_sprite_sheet.png";
    movable_entitiy_load_sprites(base, spritesheet);
}

void peach_cleanup(movable_entity_t *base) {
    movable_entitiy_cleanup(base);
}

void peach_draw(cairo_t *cr, movable_entity_t *base) {
    movable_entitiy_draw(cr, base);
}

void peach_update(peach_t *peach, float dt_seconds) {
    animation_t *animation = &peach->base.animation;
    animation_sequence_t sequence = animations[animation->current_animation];

    peach->frame_time += dt_seconds;

    if (animation->current_frame_index == sequence.frame_count - 1) {
        if (peach->frame_time < peach->anim_interval) {
            return;
        }

        // Restart animation
        peach->frame_time = 0;
        animation->current_frame_index = 0;
        animation->frame_time = 0;
        update_animation_frame(animation);
        return;
    }

    // Normal animation progress
    update_animation_progress(animation, dt_seconds);
}
