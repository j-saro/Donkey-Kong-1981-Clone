#include "movable_entity.h"
#include <gtk/gtk.h>
#include "animation.h"


void movable_entitiy_load_sprites(movable_entity_t *base, const char *spritesheet);
void movable_entitiy_cleanup(movable_entity_t *base);
void movable_entitiy_draw(cairo_t *cr, movable_entity_t *base);

void movable_entitiy_load_sprites(movable_entity_t *base, const char *spritesheet) {
    // Load Spritesheet
    base->animation.sprite_sheet = cairo_image_surface_create_from_png(spritesheet);
    if (cairo_surface_status(base->animation.sprite_sheet) != CAIRO_STATUS_SUCCESS) {
        g_warning("Failed to create '%s' spritesheet", spritesheet);
        return;
    }
    
    // Load first animation
    update_animation_frame(&base->animation);
    if (base->animation.current_frame == NULL || 
        cairo_surface_status(base->animation.current_frame) != CAIRO_STATUS_SUCCESS) {
        g_warning("Failed to create initial frame surface for '%s", spritesheet);
    }
}

void movable_entitiy_cleanup(movable_entity_t *base) {
    // Cleanup current frame
    if (base->animation.current_frame != NULL) {
        cairo_surface_destroy(base->animation.current_frame);
        base->animation.current_frame = NULL;
    }

    // Cleanup spritesheet
    if (base->animation.sprite_sheet != NULL) {
        cairo_surface_destroy(base->animation.sprite_sheet);
        base->animation.sprite_sheet = NULL;
    }
}

void movable_entitiy_draw(cairo_t *cr, movable_entity_t *base) {
    cairo_save(cr);

    float scale_x = SCALE * base->direction;
    float scale_y = SCALE;

    float draw_x = base->direction == -1 ? base->x + TILE_SIZE * SCALE : base->x;

    cairo_translate(cr, draw_x, base->y);
    cairo_scale(cr, scale_x, scale_y);

    cairo_pattern_t *pattern = cairo_pattern_create_for_surface(base->animation.current_frame);
    cairo_pattern_set_filter(pattern, CAIRO_FILTER_NEAREST); 

    cairo_set_source(cr, pattern);
    cairo_paint(cr);

    cairo_pattern_destroy(pattern);
    cairo_restore(cr);
}