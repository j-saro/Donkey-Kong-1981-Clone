#include <gtk/gtk.h>
#include "cJSON.h"
#include "consts.h"
#include "entities/abstract/entity.h"
#include "core/sprite/animation.h"
#include "core/sprite/sprite_utils.h"

void entity_parse(entity_t *base, cJSON *json);
void entity_draw(cairo_t *cr, const entity_t *base);
void entity_array_cleanup(void **array, int *count);

void entity_parse(entity_t *base, cJSON *json) {
    // Json read Values
    cJSON *type_str = cJSON_GetObjectItem(json, "type");
    if (cJSON_IsString(type_str)) {
        const char *id_str = type_str->valuestring;
        base->type = get_type_by_name(id_str);
    }

    const char *key_str = cJSON_GetObjectItem(json, "key")->valuestring;
    animation_state_t anim_state = get_type_by_name(key_str);
    if (anim_state == -1) {
        g_warning("Unknown animation state: %s", key_str);
    }
    base->animation.current_animation = anim_state;

    base->x = (float)cJSON_GetObjectItem(json, "x")->valuedouble;
    base->y = (float)cJSON_GetObjectItem(json, "y")->valuedouble;
    base->width = cJSON_GetObjectItem(json, "width")->valueint * SCALE;
    base->height = cJSON_GetObjectItem(json, "height")->valueint * SCALE;

    cJSON *dir_item = cJSON_GetObjectItem(json, "direction");
    base->direction = (dir_item && cJSON_IsNumber(dir_item)) ? dir_item->valueint : 1;


    // Default Values
    base->velocity_x = 0;
    base->velocity_y = 0;
    base->is_grounded = true;

    base->animation.current_animation_index = -1;
    base->animation.current_frame_index = 0;
    base->animation.frame_time = 0;

    set_animation_frames(base);
}


void entity_draw(cairo_t *cr, const entity_t *base) {
    cairo_save(cr);

    float scale_x = SCALE * base->direction;
    float scale_y = SCALE;

    float draw_x = base->direction == -1 ? base->x + base->width : base->x;

    cairo_translate(cr, draw_x, base->y);
    cairo_scale(cr, scale_x, scale_y);

    cairo_surface_t *frame_surface = base->animation.current_frame;
    if (frame_surface != NULL) {
        cairo_pattern_t *pattern = cairo_pattern_create_for_surface(frame_surface);
        cairo_pattern_set_filter(pattern, CAIRO_FILTER_NEAREST);

        cairo_set_source(cr, pattern);
        cairo_paint(cr);

        cairo_pattern_destroy(pattern);
    }
    else if (base->animation.current_animation == ANIM_HIDE) {}
    else {
        g_warning("(entity draw) Invalid frame surface for entity type %d, animation %d, frame %d", base->type, base->animation.current_animation, base->animation.current_frame_index);
    }

    cairo_restore(cr);
}

void entity_array_cleanup(void **array, int *count) {
    if (*array) {
        free(*array);
        *array = NULL;
    }
    *count = 0;
}