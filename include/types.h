#pragma once

#include <cairo.h>
#include <stdbool.h>

#define BASE_WIDTH 600.0f
#define BASE_HEIGHT 600.0f
#define GRAVITY 800.0f
#define MOVE_SPEED 150.0f
#define JUMP_FORCE 250.0f
#define SCALE 2.0f // player scale
#define TILE_SIZE 16
#define PLAYER_HEIGHT (TILE_SIZE * SCALE)
#define PLAYER_WIDTH (TILE_SIZE * SCALE)

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
    // Platforms
    int num_platforms;
    structure_t *platforms;
    cairo_surface_t *platform_sprite_sheet;

    // Ladders
    int num_ladders;
    structure_t *ladders;
    cairo_surface_t *ladder_sprite_sheet;
} level_t;


// Player/Mario
typedef struct {
    // Position
    float x, y;
    float previous_y; // last frame player y-pos
    float velocity_x;
    float velocity_y;
    int direction; // direction character is facing
    bool is_grounded; // if character is on ground/platform

    /* ladder related */ 
    bool on_ladder; // true if on leader else false
    bool climbing; // true if going up or down else false
    int current_platform_index;
    int current_ladder_index;

    // Sprite/Animation
    cairo_surface_t *sprite_sheet;
    cairo_surface_t *current_frame;
    int current_animation;
    int current_frame_index;
    float frame_time; // Time accumulated since last frame changed
    int frame_width, frame_height;
} player_t;

// current game state
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
    ANIM_CLIMB,
    ANIM_CLIMB_TOP,
    ANIM_CLIMB_IDLE,
} AnimationState;

typedef struct {
    int start_x;
    int start_y;
    int frame_count;
    float frame_duration;
} AnimationSequence;
