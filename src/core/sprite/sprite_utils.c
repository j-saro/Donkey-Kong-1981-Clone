#include "core/sprite/sprite_utils.h"
#include "core/sprite/sprite.h"

GHashTable* get_string_to_enum_table();
int find_animation_index(entity_t *base, animation_state_t anim_state);
animation_sequence_t get_animation_by_key(entity_t *base, animation_state_t anim_state);
cairo_surface_t **get_animation_frames(entity_t *base, animation_state_t anim_state);
void set_animation_frames(entity_t *base);
cairo_surface_t *get_spritesheet(entities_t entity);
int get_type_by_name(const char *name);


// Hashtable to map string to enum
GHashTable* get_string_to_enum_table() {
    static GHashTable* table = NULL;
    if (!table) {
        table = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
        #define X(name) g_hash_table_insert(table, g_strdup(#name), GINT_TO_POINTER(name));
            ENTITY_LIST
            ANIM_LIST
        #undef X
    }
    return table;
}

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
    // loaded_animation[animation_index]
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

    GHashTable *table = get_string_to_enum_table();
    return GPOINTER_TO_INT(g_hash_table_lookup(table, name));

    g_warning("Type did not match any name: %s", name);
    return -1;
}