#include <gtk/gtk.h>
#include "entities/abstract/entity_utils.h"

gboolean allocate_new_entity(void **array, unsigned int *num, unsigned int *capacity, size_t type_size);
void init_new_entity_base(entity_t *base, float pos_x, float pos_y, int direction);
void destroy_entity(void *array, unsigned int *count, size_t entity_size, int index);

gboolean allocate_new_entity(void **array, unsigned int *num, unsigned int *capacity, size_t type_size) {
    // check array capacity
    if (*num + 1 > *capacity) {
        // grows by 4
        unsigned int new_capacity = *capacity == 0 ? 4 : *capacity + 4;
        void *tmp = realloc(*array, new_capacity * type_size);

        if (!tmp) {
            g_warning("Failed to allocate space for new entity");
            return FALSE;
        }

        *array = tmp;
        *capacity = new_capacity;
    }
    return TRUE;
}

void init_new_entity_base(entity_t *base, float pos_x, float pos_y, int direction) {
    base->x = pos_x;
    base->y = pos_y;
    base->direction = direction;
    base->velocity_x = 0;
    base->velocity_y = 0;
    base->is_grounded = false;

    base->animation.current_frame = NULL;
    base->animation.frames = NULL;
    base->animation.current_animation_index = -1;
    base->animation.current_frame_index = 0;
    base->animation.frame_time = 0;
}

void destroy_entity(void *array, unsigned int *count, size_t entity_size, int index) {
    // Check if index is within valid bounds
    if (index < 0 || index >= (int)(*count)) {
        g_warning("Tried to destroy entity at invalid index: %d", index);
        return;
    }

    // Cast array to char* to allow byte-wise pointer arithmetic
    char *char_array = (char*)array;

    // Shift all entities after the one being removed to the left
    for (int i = index; i < (int)(*count) - 1; i++) {
        // Destination: current entity's memory location
        void *dest = char_array + i * entity_size;

        // Source: next entity's memory location
        void *src  = char_array + (i + 1) * entity_size;

        // Copy the next entity into the current one (shift left)
        memcpy(dest, src, entity_size);
    }

    // Decrease the entity count after removal
    (*count)--;
}
