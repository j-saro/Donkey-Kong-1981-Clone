#include "entities/characters/donkey_kong.h"
#include "entities/abstract/entity.h"
#include "core/sprite/animation.h"
#include "core/sprite/sprite.h"
#include "entities/abstract/static_entity.h"
#include "core/sprite/sprite_utils.h"

void donkey_kong_init(donkey_kong_t *donkey_kong, cJSON *json);
void donkey_kong_draw(cairo_t *cr, const entity_t *base);
void donkey_kong_update(game_state_t *game_state, float dt_seconds);

void donkey_kong_init(donkey_kong_t *donkey_kong, cJSON *json) {
    donkey_kong->base.type = DONKEY_KONG;
    
    // Load Values form Json
    entity_parse(&donkey_kong->base, json);

    // Donkey Kong Default Values
    donkey_kong->throw = false;
    donkey_kong->has_thrown_this_cicle = false;
}

void donkey_kong_draw(cairo_t *cr, const entity_t *base) {
    entity_draw(cr, base);
}

void donkey_kong_update(game_state_t *game_state, float dt_seconds) {
    animation_t *animation = &game_state->level.donkey_kong.base.animation;
    donkey_kong_t *donkey_kong = &game_state->level.donkey_kong;
    
    update_animation_progress(&donkey_kong->base, dt_seconds);

    if (game_state->current_cutscene > 1) return;

    animation_sequence_t sequence = get_animation_by_key(&donkey_kong->base, animation->current_animation);

    switch (animation->current_animation) {
        case ANIM_THROWING_BARREL_DONKEY_KONG:
            if (animation->current_frame_index == 1 && animation->frame_time < 0.05) {
                show_static_entity(&game_state->level);
            }
            if (animation->current_frame_index == sequence.frame_count - 1
                && animation->frame_time < 0.05) {
                hide_static_entity(&game_state->level);
            }
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