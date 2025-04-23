#include "entities/characters/barrel.h"
#include "entities/abstract/movable_entity.h"
#include <gtk/gtk.h>
#include "core/animation.h"

void barrel_init(barrel_t *barrel);
void barrel_cleanup(barrel_t *barrel);
void barrel_load_sprite(enemy_t *enemy, int i);

void barrel_init(barrel_t *barrel) {
    animation_t *animation = &barrel->animation;
    const char *filename = "./assets/mario_sprite_sheet.png"; // filler value

    animation->sprite_sheet = cairo_image_surface_create_from_png(filename);
    if (cairo_surface_status(animation->sprite_sheet) != CAIRO_STATUS_SUCCESS) {
        g_warning("Failed to create '%s' spritesheet", filename);
        return;
    }
    animation->current_frame = NULL;
    
    animation->current_animation = ANIM_CLIMB_MARIO; // filler value
    animation->frame_height = 16; // filler value
    animation->frame_width = 16; // filler value
}

void barrel_cleanup(barrel_t *barrel) {
    movable_entity_cleanup(&barrel->animation);
}

void barrel_load_sprite(enemy_t *enemy, int i) {
    update_animation_frame(&enemy->base.animation);
    if (enemy->base.animation.current_frame == NULL || 
        cairo_surface_status(enemy->base.animation.current_frame) != CAIRO_STATUS_SUCCESS) {
        g_warning("Failed to create initial frame surface for 'enemy: %d'", i);
    }
}