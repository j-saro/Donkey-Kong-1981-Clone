#pragma once

#include <cairo.h>
#include <stdbool.h>

#define BASE_WIDTH 600.0f
#define BASE_HEIGHT 600.0f
#define GRAVITY 800.0f
#define MOVE_SPEED 150.0f
#define JUMP_FORCE 300.0f
#define SCALE 2.0f
#define TILE_SIZE 16
#define PLAYER_HEIGHT (TILE_SIZE * SCALE)
#define PLAYER_WIDTH (TILE_SIZE * SCALE)

// Player/Mario
typedef struct {
    float x, y;
    float previous_y;
    float velocity_x;
    float velocity_y;
    int direction;
    bool is_grounded;

    cairo_surface_t *sprite_sheet;
    cairo_surface_t *current_frame;
    int current_animation;
    int current_frame_index;
    float frame_time; // Time accumulated since last frame changed
    int frame_width, frame_height;
} player_t;


// Level related

typedef struct {
    float x, y;
} point_t;

typedef struct {
    point_t position;
    float width;
    float height;
} structure_t;

typedef struct {
    int num_platforms;
    structure_t *platforms;
    cairo_surface_t *platform_sprite_sheet;

    int num_ladders;
    structure_t *ladders;
    cairo_surface_t *ladder_sprite_sheet;
} level_t;

typedef struct {
    player_t player;
    int *pressed_keys;
    unsigned int num_pressed_keys;
    level_t level;
} game_state_t;


// Animation related

typedef enum {
    ANIM_IDLE,
    ANIM_WALK,
    ANIM_JUMP,
    ANIM_LAND,
} AnimationState;

typedef struct {
    int start_x;
    int start_y;
    int frame_count;
    float frame_duration;
} AnimationSequence;
