#include "entities/abstract/movable_entity.h"
#include <gtk/gtk.h>
#include "core/animation.h"
#include "cJSON.h"
#include "core/sprite.h"

void movable_entity_parse(movable_entity_t *base, cJSON *json);
void movable_entity_draw(cairo_t *cr, const movable_entity_t *base);

void movable_entity_parse(movable_entity_t *base, cJSON *json) {
    // Json read Values
    cJSON *id_item = cJSON_GetObjectItem(json, "id");
    if (cJSON_IsString(id_item)) {
        const char *id_str = id_item->valuestring;
        base->type = get_type_by_name(id_str);
    }

    const char *key_str = cJSON_GetObjectItem(json, "key")->valuestring;
    base->animation.current_animation = get_type_by_name(key_str);

    base->x = (float)cJSON_GetObjectItem(json, "x")->valuedouble;
    base->y = (float)cJSON_GetObjectItem(json, "y")->valuedouble;
    base->direction = cJSON_GetObjectItem(json, "direction")->valueint;
    base->width = cJSON_GetObjectItem(json, "width")->valueint;
    base->height = cJSON_GetObjectItem(json, "height")->valueint;

    // Default Values
    base->velocity_x = 0;
    base->velocity_y = 0;
    base->is_grounded = true;

    base->animation.current_frame_index = 0;
    base->animation.frame_time = 0;
}


void movable_entity_draw(cairo_t *cr, const movable_entity_t *base) {
    cairo_save(cr);

    float scale_x = SCALE * base->direction;
    float scale_y = SCALE;

    float draw_x = base->direction == -1 ? base->x + TILE_SIZE * SCALE : base->x;

    cairo_translate(cr, draw_x, base->y);
    cairo_scale(cr, scale_x, scale_y);

    cairo_surface_t *frame_surface = animation_frame_sprites[base->type][base->animation.current_animation][base->animation.current_frame_index];
    if (frame_surface != NULL) {
        cairo_pattern_t *pattern = cairo_pattern_create_for_surface(frame_surface);
        cairo_pattern_set_filter(pattern, CAIRO_FILTER_NEAREST);

        cairo_set_source(cr, pattern);
        cairo_paint(cr);

        cairo_pattern_destroy(pattern);
    } else {
        g_warning("Invalid frame surface for entity type %d, animation %d, frame %d", base->type, base->animation.current_animation, base->animation.current_frame_index);
    }

    cairo_restore(cr);
}