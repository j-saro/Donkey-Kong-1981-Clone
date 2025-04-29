#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include "entities/abstract/static_entity.h"
#include "entities/abstract/geometry.h"
#include "entities/abstract/entity.h"
#include "core/sprite.h"
#include "core/animation.h"

void static_entity_init(level_t *level, cJSON *static_entities_json);
void static_entity_cleanup(level_t *level);
void hide_static_entity(level_t *level);
void show_static_entity(level_t *level);
void static_entity_draw(cairo_t *cr, const level_t *level);

void static_entity_init(level_t *level, cJSON *static_entities_json) {
    level->num_static_entities = cJSON_GetArraySize(static_entities_json);
    level->static_entities = calloc(level->num_static_entities, sizeof(static_entity_t));
    
    if (!level->static_entities) {
        g_warning("Out of memory allocating %d static entities",
                  level->num_static_entities);
        level->num_static_entities = 0;
        return;
    }

    for (int i = 0; i < level->num_static_entities; i++) {
        static_entity_t *static_entity = &level->static_entities[i];
        static_entity->base.type = STATIC_ENTITY;

        cJSON *entity_json = cJSON_GetArrayItem(static_entities_json, i);
        entity_parse(&static_entity->base, entity_json);
    }
}


void static_entity_cleanup(level_t *level) {
    // Cleanup static_entities array
    free(level->static_entities);
    level->static_entities = NULL;
    level->num_static_entities = 0;
}

void hide_static_entity(level_t *level) {
    for (int i = 0; i < level->num_static_entities; i++) {
        if (level->static_entities[i].base.animation.current_animation == ANIM_BARREL_FRONT_IDLE) {
            set_animation(&level->static_entities[i].base, ANIM_STATIC_ENTITY_HIDE);
        }
    }
}

void show_static_entity(level_t *level) {
    for (int i = 0; i < level->num_static_entities; i++) {
        if (level->static_entities[i].base.animation.current_animation == ANIM_STATIC_ENTITY_HIDE) {
            set_animation(&level->static_entities[i].base, ANIM_BARREL_FRONT_IDLE);
        }
    }
}

void static_entity_draw(cairo_t *cr, const level_t *level) {
    for (int i = 0; i < level->num_static_entities; ++i) {
        entity_draw(cr, &level->static_entities[i].base);
    }
}