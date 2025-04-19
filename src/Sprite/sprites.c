#include <assert.h>
#include "sprites.h"
#include "animation.h"


void sprites_load(gpointer user_data);
void sprites_cleanup(game_state_t *game_state);

void sprites_load(gpointer user_data) {
    game_state_t *game_state = (game_state_t*) user_data;

    game_state->player.sprite_sheet = cairo_image_surface_create_from_png("./assets/mario_sprite_sheet.png");
    assert(cairo_surface_status(game_state->player.sprite_sheet) == CAIRO_STATUS_SUCCESS);

    
    update_aimation_frame(&game_state->player);
    if (cairo_surface_status(game_state->player.current_frame) != CAIRO_STATUS_SUCCESS) {
        g_warning("Failed to create initial frame surface");
        sprites_cleanup(game_state);
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
}