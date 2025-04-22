#include <stdio.h>
#include <stdlib.h>
#include "entities/characters/enemy.h"
#include "entities/abstract/movable_entity.h"
#include "core/animation.h"

void enemy_init(level_t *level) {
    level->num_enemies = 0;
    level->enemy_capacity = 0;
    level->enemies = NULL;
}

void enemy_load_sprites(level_t *level) {
    const char *spritesheet = "./assets/peach_sprite_sheet.png";
    
    level->enemies_stylesheet = cairo_image_surface_create_from_png(spritesheet);
    if (cairo_surface_status(level->enemies_stylesheet) != CAIRO_STATUS_SUCCESS) {
        g_warning("Failed to create '%s' spritesheet", spritesheet);
        return;
    }
}

void new_enemy(level_t *level, cairo_surface_t *spritesheet, float pos_x, float pos_y, animation_state_t start_animation, float frame_width, float frame_height) {
    if (level->num_enemies + 1 > level->enemy_capacity) {
        unsigned int new_capacity = level->enemy_capacity == 0 ? 4 : level->enemy_capacity + 4;

        enemy_t *tmp = realloc(level->enemies, new_capacity * sizeof(enemy_t));

        if (!tmp) {
            g_warning("Failed to allocate space for new enemy");
            return;
        }

        level->enemies = tmp;
        level->num_enemies += 1;
        level->enemy_capacity = new_capacity;
    }

    enemy_t *enemy = &level->enemies[level->num_enemies - 1];

    // Position
    enemy->base.x = pos_x;
    enemy->base.y = pos_y;
    enemy->base.velocity_x = 0;
    enemy->base.velocity_y = 0;
    enemy->base.direction = 1;
    enemy->base.is_grounded = false;

    // Animation
    enemy->base.animation.current_animation = start_animation;
    enemy->base.animation.current_frame_index = 0;
    enemy->base.animation.frame_time = 0;
    enemy->base.animation.frame_width = frame_width;
    enemy->base.animation.frame_height = frame_height;

    enemy->base.animation.sprite_sheet = spritesheet;
    enemy->base.animation.current_frame = NULL;
}

void enemy_cleanup(level_t *level) {
    for (int i = 0; i < level->num_enemies; i++) {
        movable_entity_cleanup(&level->enemies[i].base);
    }

    free(level->enemies);
    level->enemies = NULL;
    level->enemy_capacity = 0;
    level->num_enemies = 0;
}

void enemy_draw(cairo_t *cr, level_t *level) {
    for (int i = 0; i < level->num_enemies; i++) { 
        movable_entity_draw(cr, &level->enemies[i].base);
    }
}

void enemy_update(level_t *level, float dt_seconds) {
    for (int i = 0; i < level->num_enemies; i++) { 
        update_animation_progress(&level->enemies[i].base.animation, dt_seconds);
    }
}
