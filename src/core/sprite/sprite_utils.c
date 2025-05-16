#include "core/sprite/sprite_utils.h"
#include "core/sprite/sprite.h"
#include <gtk/gtk.h>

int find_animation_index(entity_t *base, animation_state_t anim_state);
animation_sequence_t get_animation_by_key(entity_t *base, animation_state_t anim_state);
cairo_surface_t **get_animation_frames(entity_t *base, animation_state_t anim_state);
void set_animation_frames(entity_t *base);
cairo_surface_t *get_spritesheet(entities_t entity);
int get_type_by_name(const char *name);

static const string_to_enmum_t global_type_map[] = {
    // Entities
    {"MARIO", MARIO},
    {"DONKEY_KONG", DONKEY_KONG},
    {"PEACH", PEACH},
    {"ENEMY", ENEMY},
    {"HAMMER", HAMMER},
    {"LADDER", LADDER},
    {"PLATFORM", PLATFORM},
    {"STATIC_ENTITY", STATIC_ENTITY},
    {"EFFECT", EFFECT},

    // Animations
    // Mario
    {"ANIM_IDLE_MARIO", ANIM_IDLE_MARIO},
    {"ANIM_WALK_MARIO", ANIM_WALK_MARIO},
    {"ANIM_JUMP_MARIO", ANIM_JUMP_MARIO},
    {"ANIM_CLIMB_MARIO", ANIM_CLIMB_MARIO},
    {"ANIM_CLIMB_IDLE_MARIO", ANIM_CLIMB_IDLE_MARIO},
    {"ANIM_HAMMER_MARIO_STAND", ANIM_HAMMER_MARIO_STAND},
    {"ANIM_HAMMER_MARIO_WALK", ANIM_HAMMER_MARIO_WALK},

    // Peach
    {"ANIM_IDLE_PEACH", ANIM_IDLE_PEACH},
    {"ANIM_HELP_PEACH", ANIM_HELP_PEACH},

    // Donkey Kong
    {"ANIM_IDLE_DONKEY_KONG", ANIM_IDLE_DONKEY_KONG},
    {"ANIM_BEATING_CHEST_DONKEY_KONG", ANIM_BEATING_CHEST_DONKEY_KONG},
    {"ANIM_THROWING_BARREL_DONKEY_KONG", ANIM_THROWING_BARREL_DONKEY_KONG},
    {"ANIM_CLIMB_WITH_PEACH_DONKEY_KONG", ANIM_CLIMB_WITH_PEACH_DONKEY_KONG},
    {"ANIM_CLIMB_WITHOUT_PEACH_DONKEY_KONG", ANIM_CLIMB_WITHOUT_PEACH_DONKEY_KONG},

    // Enemys
    {"ANIM_BARREL_SIDE", ANIM_BARREL_SIDE},
    {"ANIM_BARREL_FRONT_IDLE", ANIM_BARREL_FRONT_IDLE},
    {"ANIM_BARREL_FRONT", ANIM_BARREL_FRONT},
    {"ANIM_FIRE_SPIRIT_WALK", ANIM_FIRE_SPIRIT_WALK},
    {"ANIM_IDLE_LORE", ANIM_IDLE_LORE},

    // Static entitys
    {"ANIM_OIL_BARREL", ANIM_OIL_BARREL},
    {"ANIM_BARREL_STACK", ANIM_BARREL_STACK},
    {"ANIM_STATIC_HAMMER", ANIM_STATIC_HAMMER},
    {"ANIM_UMBRELLA_ITEM", ANIM_UMBRELLA_ITEM},
    {"ANIM_HANDBAG_ITEM", ANIM_HANDBAG_ITEM},
    {"ANIM_HAT_ITEM", ANIM_HAT_ITEM},
    {"ANIM_FIRE_SMALL", ANIM_FIRE_SMALL},
    {"ANIM_FIRE_LARGE", ANIM_FIRE_LARGE},
    {"ANIM_CONVEYER_ROLLER_LEFT", ANIM_CONVEYER_ROLLER_LEFT},
    {"ANIM_CONVEYER_ROLLER_RIGHT", ANIM_CONVEYER_ROLLER_RIGHT},
    {"ANIM_NUMBERS", ANIM_NUMBERS},
    {"ANIM_ALPHABET", ANIM_ALPHABET},
    {"ANIM_SPECIAL", ANIM_SPECIAL},
    {"ANIM_LIVES", ANIM_LIVES},
    {"ANIM_BONUS_POINTS", ANIM_BONUS_POINTS},

    // Effects
    {"ANIM_ENEMY_DEATH", ANIM_ENEMY_DEATH},
    {"ANIM_HEART_FULL", ANIM_HEART_FULL},
    {"ANIM_HEART_BROKEN", ANIM_HEART_BROKEN},

    // Ladders
    {"ANIM_LADDER_WHITE", ANIM_LADDER_WHITE},
    {"ANIM_LADDER_BLUE", ANIM_LADDER_BLUE},
    {"ANIM_LADDER_YELLOW", ANIM_LADDER_YELLOW},

    // Platforms
    {"ANIM_PLATFORM_RED", ANIM_PLATFORM_RED},
    {"ANIM_PLATFORM_YELLOW", ANIM_PLATFORM_YELLOW},
    {"ANIM_PLATFORM_CONVEYER_BELT", ANIM_PLATFORM_CONVEYER_BELT},

    // No Animation
    {"ANIM_HIDE", ANIM_HIDE},
};

// get number of elements in map
#define NUM_ENTRIES (sizeof(global_type_map) / sizeof(global_type_map[0]))

int find_animation_index(entity_t *base, animation_state_t anim_state) {
    // return if index is cached
    if (base->animation.current_animation == anim_state &&
        base->animation.current_animation_index != -1) {
        return base->animation.current_animation_index;
    }

    // find animation index in array
    for (int i = 0; i < num_animations; i++) {
        if (animations[i].anim_key == anim_state) {
            base->animation.current_animation_index = i;
            return i;
        }
    }
    return -1;
}

// returns animation sequence by animation state
animation_sequence_t get_animation_by_key(entity_t *base, animation_state_t anim_state) {
    int index = find_animation_index(base, anim_state);
    if (index >= 0) return animations[index];
    g_warning("Animation not found for state %d", anim_state);
    animation_sequence_t empty = {0};
    return empty;
}

// returns animation frames array of current animation
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

    // set animation frames array of current animation
    base->animation.frames = get_animation_frames(base, base->animation.current_animation);
    int index = base->animation.current_frame_index;
    if (base->animation.frames != NULL && index >= 0) {
        base->animation.current_frame = base->animation.frames[index];
    } else {
        g_warning("(set animation frames) Invalid animation frame access: state=%d, index=%d", base->animation.current_animation, index);
        base->animation.current_frame = NULL;
    }
}

// retruns spritesheet of entities type
cairo_surface_t *get_spritesheet(entities_t entity) {
    for (int i = 0; i < num_sprite_sheets; i++) {
        if (sprite_sheets[i].type == entity) {
            return sprite_sheets[i].sprite_sheet;
        }
    }
    g_warning("Spritesheet not found for %d", entity);
    return NULL;
}


// helper function to map string to enum
int get_type_by_name(const char *name) {
    if (name == NULL) {
        g_warning("get_type_by_name called with NULL name input.");
        return -1;
    }

    for (size_t i = 0; i < NUM_ENTRIES; ++i) {
        if (strcmp(name, global_type_map[i].name) == 0) {
            return global_type_map[i].value;
        }
    }

    g_warning("Type did not match any name: %s", name);
    return -1;
}