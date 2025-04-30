#include "entities/abstract/item.h"
#include <stdio.h>
#include <stdlib.h>
#include "core/utils.h"

void item_init(level_t *level, cJSON *json) {
    level->num_items = cJSON_GetArraySize(json);
    level->items = calloc(level->num_items, sizeof(item_t));

    for (int i = 0; i < level->num_items; i++) {
        cJSON *item_json = cJSON_GetArrayItem(json, i);
        item_t *item = &level->items[i];
        
        const char *id_str = cJSON_GetObjectItem(item_json, "id")->valuestring;
        item->base.type = get_type_by_name(id_str);
        item->x = cJSON_GetObjectItem(item_json, "x")->valuedouble;
        item->y = cJSON_GetObjectItem(item_json, "y")->valuedouble;
        item->points = cJSON_GetObjectItem(item_json, "points")->valueint;
    }
}


void item_cleanup(level_t *level) {
    free(level->items);
    level->items = NULL;
    level->num_items = 0;
}