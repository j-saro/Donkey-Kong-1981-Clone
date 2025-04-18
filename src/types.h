#pragma once

#include <cairo.h>
#include <stdbool.h>

#define GRAVITY 1000.0f
#define GROUND_LEVEL 0.0f 
#define MOVE_SPEED 200.0f
#define JUMP_FORCE 500.0f
#define SCALE 4.0f
#define TILE_SIZE 16
#define PLAYER_HEIGHT (TILE_SIZE * SCALE)
#define PLAYER_WIDTH (TILE_SIZE * SCALE)

typedef struct {
    float x, y;
    float velocity_x;
    float velocity_y;
    cairo_surface_t *sprite_sheet;
    cairo_surface_t *sprite;
    bool is_grounded;
} player_t;

typedef struct {
    player_t player;
    int *pressed_keys;
    unsigned int num_pressed_keys;
} game_state_t;
