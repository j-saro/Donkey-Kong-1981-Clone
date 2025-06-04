#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include "entities/environment/effect.h"
#include "entities/abstract/entity.h"
#include "entities/abstract/entity_utils.h"
#include "core/sprite/sprite_utils.h"
#include "core/sprite/animation.h"

void effect_init(level_t *level);
void new_effect(level_t *level, animation_state_t animation, float pos_x, float pos_y, int direction, bool once);
void effect_destroy(level_t *level, int index);
void effect_clear_all(level_t *level);
void effect_cleanup(level_t *level);
void effect_update(level_t *level, float dt_seconds);
bool effect_is_active(level_t *level, animation_state_t anim);
void effect_draw(cairo_t *cr, const level_t *level);

void effect_init(level_t *level) {
    level->effects = NULL;
    level->effects_capacity = 0;
    level->num_effects = 0;
}

void new_effect(level_t *level, animation_state_t animation, float pos_x, float pos_y, int direction, bool once) {
    if (!allocate_new_entity((void**)&level->effects, &level->num_effects, &level->effects_capacity, sizeof(effect_t))) {
        return;
    }

    level->num_effects += 1;
    effect_t *effect = &level->effects[level->num_effects - 1];

    init_new_entity_base(&effect->base, pos_x, pos_y, direction);
    effect->base.type = EFFECT;
    effect->base.animation.current_animation = animation;
    effect->once = once;
 
    set_animation_frames(&effect->base);
}

void effect_destroy(level_t *level, int index) {
    destroy_entity(level->effects, &level->num_effects, sizeof(effect_t), index);
}

void effect_clear_all(level_t *level) {
    for (int i = 0; i < level->num_effects; i++) {
        effect_destroy(level, i);
        i--;
    }
}

void effect_cleanup(level_t *level) {
    entity_array_cleanup((void**)&level->effects, &level->num_effects);
    level->effects_capacity = 0;
}

void effect_update(level_t *level, float dt_seconds) {
    for (int i = 0; i < level->num_effects; i++) {
        effect_t *effect = &level->effects[i];
        animation_sequence_t sequence = get_animation_by_key(&effect->base, effect->base.animation.current_animation);
        update_animation_progress(&effect->base, dt_seconds);

        // if effect at last frame and frame is shown, destroy it
        if (effect->base.animation.current_frame_index == (sequence.frame_count - 1) && 
            effect->base.animation.frame_time >= sequence.frame_duration - (dt_seconds * 2) &&
            effect->once) {
            effect_destroy(level, i);
            i--;
            continue;
        }
    }
}

bool effect_is_active(level_t *level, animation_state_t anim) {
    for (int i = 0; i < level->num_effects; i++) {
        effect_t *effect = &level->effects[i];
        
        if (effect->base.animation.current_animation == ANIM_ENEMY_DEATH) {
            return TRUE;
            break;
        }
    }
    return FALSE;
}

void effect_draw(cairo_t *cr, const level_t *level) {
    for (int i = 0; i < level->num_effects; i++) { 
        entity_draw(cr, &level->effects[i].base);
    }
}