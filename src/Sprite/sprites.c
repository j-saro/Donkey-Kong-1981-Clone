#include <assert.h>
#include "sprites.h"
#include "animation.h"


void load_sprites(gpointer user_data) {
    game_state_t *game_state = (game_state_t*) user_data;

    game_state->player.sprite_sheet = cairo_image_surface_create_from_png("./assets/mario_sprite_sheet.png");
    assert(cairo_surface_status(game_state->player.sprite_sheet) == CAIRO_STATUS_SUCCESS);

    
    update_player_frame(&game_state->player);
    if (cairo_surface_status(game_state->player.current_frame) != CAIRO_STATUS_SUCCESS) {
        g_warning("Failed to create initial frame surface");
        cairo_surface_destroy(game_state->player.sprite_sheet);
    }
}

void cleanup_sprites(player_t *player) {
    if (player->current_frame != NULL) {
        cairo_surface_destroy(player->current_frame);
        player->current_frame = NULL;
    }
    
    if (player->sprite_sheet != NULL) {
        cairo_surface_destroy(player->sprite_sheet);
        player->sprite_sheet = NULL;
    }
}