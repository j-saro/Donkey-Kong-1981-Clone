#pragma once

#include <cairo.h>
#include <stdbool.h>

#define BASE_WIDTH 600.0f
#define BASE_HEIGHT 600.0f
#define GRAVITY 800.0f
#define MOVE_SPEED 100.0f
#define JUMP_FORCE 250.0f
#define SCALE 2.0f
#define TILE_SIZE 16
#define PLAYER_HEIGHT (TILE_SIZE * SCALE)
#define PLAYER_WIDTH (TILE_SIZE * SCALE)
#define PHYSICS_EPSILON 19
#define MAX_ENEMIES 50


/* 
* Animation related
*/

typedef enum {
    MARIO,
    DONKEY_KONG,
    PEACH,
    BARREL,
    HAMMER,
    LADDER,
    PLATFORM,
    STATIC_ENTITY,

    // size
    ENTITY_COUNT
} entities_t;

typedef enum {
    // Mario
    ANIM_IDLE_MARIO,
    ANIM_WALK_MARIO,
    ANIM_JUMP_MARIO,
    ANIM_CLIMB_MARIO,
    ANIM_CLIMB_IDLE_MARIO,

    // Peach
    ANIM_IDLE_PEACH,

    // Donkey Kong
    ANIM_IDLE_DONKEY_KONG,
    ANIM_BEATING_CHEST_DONKEY_KONG,
    ANIM_THROWING_BARREL_DONKEY_KONG,

    // Barrel
    ANIM_BARREL_SIDE,
    ANIM_BARREL_FRONT,
    ANIM_BARREL_FRONT_IDLE,

    // STATIC OIL BARREL
    ANIM_OIL_BARREL,

    // Donky Kong Barrel Stack
    ANIM_BARREL_STACK,

    // Hide
    ANIM_STATIC_ENTITY_HIDE,

    // size
    ANIMATION_COUNT
} animation_state_t;

typedef struct {
    entities_t type;
    int start_x;
    int start_y;
    int frame_count;
    float frame_duration;
    int frame_width, frame_height;
} animation_sequence_t;

typedef struct {
    cairo_surface_t *current_frame;
    int current_animation;
    int current_frame_index;
    float frame_time;
} animation_t;

/* 
* Abstract 'Classes'
*/

typedef struct {
    entities_t type;
    float x, y;
    float width;
    float height;
    bool has_physics;
} geometry_t;

typedef struct {
    entities_t type;

    float x, y;
    float velocity_x, velocity_y;
    int direction;
    bool is_grounded;
    float width, height;

    animation_t animation;
} movable_entity_t;

typedef struct {
    entities_t type;
    float x, y;
    int direction;
    float spawn_interval;
    float spawn_timer;
} enemy_spawn_t;

/* 
* Level related
*/

typedef struct {
    movable_entity_t base;
    float fly_time;
} enemy_t;

typedef struct {
    movable_entity_t base;
    float x, y;
    int points;
} item_t;

typedef struct {
    movable_entity_t base;
} static_entity_t;

// Actors
typedef struct {
    movable_entity_t base;
    float anim_interval;
    float anim_time;
} peach_t;

typedef struct {
    movable_entity_t base;
    bool throw;
    bool has_thrown_this_cicle;
} donkey_kong_t;

typedef struct {
    movable_entity_t base;
    float previous_y;

    // Ladder 
    bool on_ladder;
    bool climbing;
    int current_platform_index;
    int current_ladder_index;
} player_t;


typedef struct {
    // Player/Mario
    player_t player;

    // Peach
    peach_t peach;

    // Donkey Kong
    donkey_kong_t donkey_kong;

    // Platforms
    int num_platforms;
    geometry_t *platforms;
    cairo_surface_t *platform_sprite_sheet;

    // Ladders
    int num_ladders;
    geometry_t *ladders;
    cairo_surface_t *ladder_sprite_sheet;

    // Static objects
    int num_static_entities;
    static_entity_t *static_entities;

    // Enemys
    enemy_t *enemies;
    unsigned int enemy_capacity;
    unsigned int num_enemies;

    enemy_spawn_t *enemy_spawns;
    unsigned int num_enemy_spawns;

    // Items
    item_t *items;
    unsigned int num_items;
} level_t;


/* 
* Game State
*/
typedef struct {
    int *pressed_keys;
    unsigned int num_pressed_keys;
    level_t level;

} game_state_t;
