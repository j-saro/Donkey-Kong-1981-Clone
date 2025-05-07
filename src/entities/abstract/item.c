#include "entities/abstract/item.h"
#include <stdio.h>
#include <stdlib.h>
#include "core/sprite/sprite_utils.h"
#include "entities/abstract/entity.h"
#include <gtk/gtk.h>


void item_init(level_t *level, cJSON *json);
void item_destroy(level_t *level, int index);
void item_draw(cairo_t *cr, const level_t *level);
void item_cleanup(level_t *level);

void item_init(level_t *level, cJSON *json) {
    level->num_items = cJSON_GetArraySize(json);
    level->items = calloc(level->num_items, sizeof(item_t));

    for (int i = 0; i < level->num_items; i++) {
        cJSON *item_json = cJSON_GetArrayItem(json, i);
        item_t *item = &level->items[i];
        
        entity_parse(&item->base, item_json);
        item->points = cJSON_GetObjectItem(item_json, "points")->valueint;
    }
}

void item_destroy(level_t *level, int index) {
    if (index < 0 || index >= level->num_items) {
        g_warning("Tried to destroy item at invalid index: %d", index);
        return;
    }

    // Shift all remaining items left to fill the gap
    for (int i = index; i < level->num_items - 1; i++) {
        level->items[i] = level->items[i + 1];
    }

    level->num_items--;
}

void item_draw(cairo_t *cr, const level_t *level) {
    for (int i = 0; i < level->num_items; i++) { 
        entity_draw(cr, &level->items[i].base);
    }
}

void item_cleanup(level_t *level) {
    free(level->items);
    level->items = NULL;
    level->num_items = 0;
}