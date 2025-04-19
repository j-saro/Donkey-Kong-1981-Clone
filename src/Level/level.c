#include <stdlib.h>
#include "level.h"


void level_init(level_t *level);
void level_cleanup(level_t *level);
void level_draw(cairo_t *cr, const level_t *level);

void level_init(level_t *level) {
    // Hardcoded Platfrom for testing purpose
    level->num_platforms = 1;
    level->platforms = malloc(sizeof(platform_t) * level->num_platforms);

    platform_t *platform = &level->platforms[0];
    platform->position.x = 50.0f;
    platform->position.y = 500.0f;
    platform->width = 500.0f;
    platform->height = 20.0f;
    platform->sprite = NULL;
}

void level_cleanup(level_t *level) {
    free(level->platforms);
    level->platforms = NULL;
    level->num_platforms = 0;
}

void level_draw(cairo_t *cr, const level_t *level) {
    for (int i = 0; i < level->num_platforms; ++i) {
        const platform_t *platform = &level->platforms[i];

        if (platform->sprite != NULL) {
            cairo_save(cr);
            cairo_set_source_surface(cr, platform->sprite, platform->position.x, platform->position.y);
            cairo_paint(cr);
            cairo_restore(cr);
        } else {
            cairo_save(cr);
            cairo_set_source_rgb(cr, 0.3, 0.3, 0.8);
            cairo_rectangle(cr, platform->position.x, platform->position.y,
                            platform->width, platform->height);
            cairo_fill(cr);
            cairo_restore(cr);
        }
    }
}