#include "core/sprite/sprite_utils.h"
#include "core/sprite/sprite.h"
#include <gtk/gtk.h>


int find_animation_index(entity_t *base, animation_state_t anim_state);
animation_sequence_t get_animation_by_key(entity_t *base, animation_state_t anim_state);
cairo_surface_t **get_animation_frames(entity_t *base, animation_state_t anim_state);
void set_animation_frames(entity_t *base);
cairo_surface_t *get_spritesheet(entities_t entity);
int get_type_by_name(const char *name);

int find_animation_index(entity_t *base, animation_state_t anim_state) {
    if (base->animation.current_animation == anim_state &&
        base->animation.current_animation_index != -1) {
        return base->animation.current_animation_index;
    }

    for (int i = 0; i < num_animations; i++) {
        if (animations[i].anim_key == anim_state) {
            base->animation.current_animation_index = i;
            return i;
        }
    }
    return -1;
}

animation_sequence_t get_animation_by_key(entity_t *base, animation_state_t anim_state) {
    int index = find_animation_index(base, anim_state);
    if (index >= 0) return animations[index];
    g_warning("Animation not found for state %d", anim_state);
    animation_sequence_t empty = {0};
    return empty;
}

cairo_surface_t **get_animation_frames(entity_t *base, animation_state_t anim_state) {
    int index = find_animation_index(base, anim_state);
    if (index >= 0) return loaded_animations[index].frames;
    g_warning("No animation frames found for animation state %d", anim_state);
    return NULL;
}

void set_animation_frames(entity_t *base) {
    if (base->animation.current_animation == ANIM_HIDE) {
        base->animation.current_frame = NULL;
        return;
    }

    base->animation.frames = get_animation_frames(base, base->animation.current_animation);
    int index = base->animation.current_frame_index;
    if (base->animation.frames != NULL && index >= 0) {
        base->animation.current_frame = base->animation.frames[index];
    } else {
        g_warning("(set animation frames) Invalid animation frame access: state=%d, index=%d", base->animation.current_animation, index);
        base->animation.current_frame = NULL;
    }
}

cairo_surface_t *get_spritesheet(entities_t entity) {
    for (int i = 0; i < num_sprite_sheets; i++) {
        if (sprite_sheets[i].type == entity) {
            return sprite_sheets[i].sprite_sheet;
        }
    }
    g_warning("Spritesheet not found for %d", entity);
    return NULL;
}


int get_type_by_name(const char *name) {
    // Entities
    if (strcmp(name, "MARIO") == 0) return MARIO;
    if (strcmp(name, "DONKEY_KONG") == 0) return DONKEY_KONG;
    if (strcmp(name, "PEACH") == 0) return PEACH;
    if (strcmp(name, "BARREL") == 0) return BARREL;
    if (strcmp(name, "ENEMY_DEATH") == 0) return ENEMY_DEATH;
    if (strcmp(name, "HAMMER") == 0) return HAMMER;
    if (strcmp(name, "LADDER") == 0) return LADDER;
    if (strcmp(name, "PLATFORM") == 0) return PLATFORM;
    if (strcmp(name, "STATIC_ENTITY") == 0) return STATIC_ENTITY;

    // Animations
    if (strcmp(name, "ANIM_IDLE_MARIO") == 0) return ANIM_IDLE_MARIO;
    if (strcmp(name, "ANIM_WALK_MARIO") == 0) return ANIM_WALK_MARIO;
    if (strcmp(name, "ANIM_JUMP_MARIO") == 0) return ANIM_JUMP_MARIO;
    if (strcmp(name, "ANIM_CLIMB_MARIO") == 0) return ANIM_CLIMB_MARIO;
    if (strcmp(name, "ANIM_CLIMB_IDLE_MARIO") == 0) return ANIM_CLIMB_IDLE_MARIO;
    if (strcmp(name, "ANIM_HAMMER_MARIO_STAND") == 0) return ANIM_HAMMER_MARIO_STAND;
    if (strcmp(name, "ANIM_HAMMER_MARIO_WALK") == 0) return ANIM_HAMMER_MARIO_WALK;
    if (strcmp(name, "ANIM_IDLE_PEACH") == 0) return ANIM_IDLE_PEACH;
    if (strcmp(name, "ANIM_HELP_PEACH") == 0) return ANIM_HELP_PEACH;
    if (strcmp(name, "ANIM_IDLE_DONKEY_KONG") == 0) return ANIM_IDLE_DONKEY_KONG;
    if (strcmp(name, "ANIM_BEATING_CHEST_DONKEY_KONG") == 0) return ANIM_BEATING_CHEST_DONKEY_KONG;
    if (strcmp(name, "ANIM_THROWING_BARREL_DONKEY_KONG") == 0) return ANIM_THROWING_BARREL_DONKEY_KONG;
    if (strcmp(name, "ANIM_CLIMB_WITH_PEACH_DONKEY_KONG") == 0) return ANIM_CLIMB_WITH_PEACH_DONKEY_KONG;
    if (strcmp(name, "ANIM_CLIMB_WITHOUT_PEACH_DONKEY_KONG") == 0) return ANIM_CLIMB_WITHOUT_PEACH_DONKEY_KONG;
    if (strcmp(name, "ANIM_BARREL_SIDE") == 0) return ANIM_BARREL_SIDE;
    if (strcmp(name, "ANIM_BARREL_FRONT_IDLE") == 0) return ANIM_BARREL_FRONT_IDLE;
    if (strcmp(name, "ANIM_BARREL_FRONT") == 0) return ANIM_BARREL_FRONT;
    if (strcmp(name, "ANIM_OIL_BARREL") == 0) return ANIM_OIL_BARREL;
    if (strcmp(name, "ANIM_BARREL_STACK") == 0) return ANIM_BARREL_STACK;
    if (strcmp(name, "ANIM_HIDE") == 0) return ANIM_HIDE;
    if (strcmp(name, "ANIM_STATIC_HAMMER") == 0) return ANIM_STATIC_HAMMER;
    if (strcmp(name, "ANIM_ENEMY_DEATH") == 0) return ANIM_ENEMY_DEATH;

    g_warning("Type did not match any name");
    return -1;
}