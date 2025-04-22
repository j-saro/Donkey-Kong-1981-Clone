#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include "entities/abstract/static_entity.h"
#include "entities/abstract/structure.h"

void static_entity_init(level_t *level, cJSON *static_entities_json);
void static_entity_cleanup(level_t *level);
void static_entity_draw(cairo_t *cr, const level_t *level);

void static_entity_init(level_t *level, cJSON *static_entities_json) {
    level->num_static_entities = cJSON_GetArraySize(static_entities_json);
    level->static_entities = malloc(level->num_static_entities * sizeof(static_entity_t));
    if (!level->static_entities) {
        g_warning("Out of memory allocating %d static entities",
                  level->num_static_entities);
        level->num_static_entities = 0;
        return;
    }

    for (int i = 0; i < level->num_static_entities; i++) {
        cJSON *entity_json = cJSON_GetArrayItem(static_entities_json, i);
        structure_parse(&level->static_entities[i].entity, entity_json);

        // Get spritesheet path
        cJSON *sheet_item = cJSON_GetObjectItem(entity_json, "sprite");
        if (!cJSON_IsString(sheet_item)) {
            g_warning("static_entities[%d] missing or invalid “sprite” field", i);
            level->static_entities[i].sprite = NULL;
            continue;
        }
        const char *sheet_filename = sheet_item->valuestring;
        level->static_entities[i].sprite_sheet = cairo_image_surface_create_from_png(sheet_filename);
        if (cairo_surface_status(level->static_entities[i].sprite_sheet) != CAIRO_STATUS_SUCCESS) {
            g_warning("Couldn't load '%s'", sheet_filename);
            return;
        }

        float sprite_x = (float)cJSON_GetObjectItem(entity_json, "sprite_x")->valuedouble;
        float sprite_y = (float)cJSON_GetObjectItem(entity_json, "sprite_y")->valuedouble;

        level->static_entities[i].sprite = cairo_surface_create_for_rectangle(
                level->static_entities[i].sprite_sheet,
                sprite_x,
                sprite_y,
                level->static_entities[i].entity.width,
                level->static_entities[i].entity.height
            );
    }
}

void static_entity_cleanup(level_t *level) {
    // Cleanup every static entity sprite
    for (int i = 0; i < level->num_static_entities; i++) {
        if (level->static_entities[i].sprite != NULL) {
            cairo_surface_destroy(level->static_entities[i].sprite);
            level->static_entities[i].sprite = NULL;
        }
        if (level->static_entities[i].sprite_sheet != NULL) {
            cairo_surface_destroy(level->static_entities[i].sprite_sheet);
            level->static_entities[i].sprite_sheet = NULL;
        }
    }

    // Cleanup static_entities array
    free(level->static_entities);
    level->static_entities = NULL;
    level->num_static_entities = 0;
}


void static_entity_draw(cairo_t *cr, const level_t *level) {
    for (int i = 0; i < level->num_static_entities; ++i) {
        const static_entity_t *entity = &level->static_entities[i];

        cairo_save(cr);

        cairo_translate(cr, entity->entity.x, entity->entity.y);
        cairo_scale(cr, SCALE, SCALE);

        cairo_pattern_t *pattern = cairo_pattern_create_for_surface(entity->sprite);
        cairo_pattern_set_filter(pattern, CAIRO_FILTER_NEAREST);

        cairo_set_source(cr, pattern);
        cairo_rectangle(cr, 0, 0, entity->entity.width, entity->entity.height);
        cairo_fill(cr);

        cairo_pattern_destroy(pattern);
        cairo_restore(cr);
    }
}