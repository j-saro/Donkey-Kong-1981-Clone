#include <stdio.h>
#include <stdlib.h>
#include "barrel.h"
#include "structure.h"

void barrel_init(level_t *level, cJSON *barrels_json);
void barrel_cleanup(level_t *level);
void barrel_draw(cairo_t *cr, const level_t *level);

void barrel_init(level_t *level, cJSON *barrels_json) {
    level->num_barrels = cJSON_GetArraySize(barrels_json);
    level->barrels = malloc(level->num_barrels * sizeof(barrel_t));

    for (int i = 0; i < level->num_barrels; i++) {
        cJSON *barrel_json = cJSON_GetArrayItem(barrels_json, i);
        structure_parse(&level->barrels[i].barrel, barrel_json);

        level->barrels[i].sprite = cairo_surface_create_for_rectangle(
            level->barrel_sprite_sheet,
            0, 7, TILE_SIZE, TILE_SIZE
        );
    }
}

void barrel_cleanup(level_t *level) {
    free(level->barrels);
    level->barrels = NULL;
    level->num_barrels = 0;
}

void barrel_draw(cairo_t *cr, const level_t *level) {
    for (int i = 0; i < level->num_barrels; ++i) {
        const barrel_t *barrel = &level->barrels[i];

        cairo_save(cr);

        cairo_translate(cr, barrel->barrel.x, barrel->barrel.y);
        cairo_scale(cr, SCALE, SCALE);

        cairo_pattern_t *pattern = cairo_pattern_create_for_surface(barrel->sprite);
        cairo_pattern_set_filter(pattern, CAIRO_FILTER_NEAREST);

        cairo_set_source(cr, pattern);
        cairo_rectangle(cr, 0, 0, barrel->barrel.width, barrel->barrel.height);
        cairo_fill(cr);

        cairo_pattern_destroy(pattern);
        cairo_restore(cr);
    }
}