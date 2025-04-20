#include <assert.h>
#include "sprites.h"
#include "animation.h"


void sprites_load_player(gpointer user_data);
void sprites_load_level(gpointer user_data);
void sprites_cleanup(game_state_t *game_state);

void sprites_load_player(gpointer user_data) {
    game_state_t *game_state = (game_state_t*) user_data;

    // Load player sprite
    game_state->player.sprite_sheet = cairo_image_surface_create_from_png("./assets/mario_sprite_sheet.png");
    if (cairo_surface_status(game_state->player.sprite_sheet) != CAIRO_STATUS_SUCCESS) {
        g_warning("Failed to create player spritesheet");
        sprites_cleanup(game_state);
        return;
    }

    // Load first animation
    update_animation_frame(&game_state->player);
    if (game_state->player.current_frame == NULL || 
        cairo_surface_status(game_state->player.current_frame) != CAIRO_STATUS_SUCCESS) {
        g_warning("Failed to create initial frame surface");
        sprites_cleanup(game_state);
    }
}

void sprites_load_level(gpointer user_data) {
    game_state_t *game_state = (game_state_t*) user_data;
    // Load level/platform sprite
    game_state->level.platform_sprite_sheet = cairo_image_surface_create_from_png("./assets/platform.png");
    if (cairo_surface_status(game_state->level.platform_sprite_sheet) != CAIRO_STATUS_SUCCESS) {
        g_warning("Failed to create platform spritesheet");
        sprites_cleanup(game_state);
        return;
    }

    game_state->level.ladder_sprite_sheet = cairo_image_surface_create_from_png("./assets/ladder.png");
    if (cairo_surface_status(game_state->level.ladder_sprite_sheet) != CAIRO_STATUS_SUCCESS) {
        g_warning("Failed to create ladder spritesheet");
        sprites_cleanup(game_state);
        return;
    }
}

void sprites_cleanup(game_state_t *game_state) {
    if (game_state->player.current_frame != NULL) {
        cairo_surface_destroy(game_state->player.current_frame);
        game_state->player.current_frame = NULL;
    }
    
    if (game_state->player.sprite_sheet != NULL) {
        cairo_surface_destroy(game_state->player.sprite_sheet);
        game_state->player.sprite_sheet = NULL;
    }

    if (game_state->level.platform_sprite_sheet != NULL) {
        cairo_surface_destroy(game_state->level.platform_sprite_sheet);
        game_state->level.platform_sprite_sheet = NULL;
    }
}