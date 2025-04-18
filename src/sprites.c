#include <assert.h>
#include "sprites.h"

void load_sprites(gpointer user_data) {
    game_state_t *game_state = (game_state_t*) user_data;

    game_state->player.sprite_sheet = cairo_image_surface_create_from_png("./assets/mario_sprite_sheet.png");
    game_state->player.sprite = cairo_surface_create_for_rectangle(game_state->player.sprite_sheet, 1, 21, TILE_SIZE, TILE_SIZE);
    assert(cairo_surface_status(game_state->player.sprite) == CAIRO_STATUS_SUCCESS);
}