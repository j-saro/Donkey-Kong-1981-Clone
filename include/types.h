#pragma once

#include <cairo.h>
#include <stdbool.h>

typedef enum {
    GAME_MODE_NORMAL,
    GAME_MODE_CUTSCENE,
    GAME_MODE_PAUSED,
    GAME_MODE_EFFECT,
} game_mode_t;

typedef enum {
    MARIO,
    DONKEY_KONG,
    PEACH,
    BARREL,
    FIRE_SPIRIT,
    HAMMER,
    LADDER,
    PLATFORM,
    STATIC_ENTITY,
    EFFECT,
} entities_t;

/* 
* Animation related
*/

typedef enum {
    // Mario
    ANIM_IDLE_MARIO,
    ANIM_WALK_MARIO,
    ANIM_JUMP_MARIO,
    ANIM_CLIMB_MARIO,
    ANIM_CLIMB_IDLE_MARIO,
    ANIM_HAMMER_MARIO_STAND,
    ANIM_HAMMER_MARIO_WALK,

    // Peach
    ANIM_IDLE_PEACH,
    ANIM_HELP_PEACH,

    // Donkey Kong
    ANIM_IDLE_DONKEY_KONG,
    ANIM_BEATING_CHEST_DONKEY_KONG,
    ANIM_THROWING_BARREL_DONKEY_KONG,
    ANIM_CLIMB_WITH_PEACH_DONKEY_KONG,
    ANIM_CLIMB_WITHOUT_PEACH_DONKEY_KONG,

    // Barrel
    ANIM_BARREL_SIDE,
    ANIM_BARREL_FRONT,
    ANIM_BARREL_FRONT_IDLE,

    // Fire spirit
    ANIM_FIRE_SPIRIT_WALK,

    // Static entitys
    ANIM_OIL_BARREL,
    ANIM_STATIC_HAMMER,
    ANIM_BARREL_STACK,
    ANIM_UMBRELLA_ITEM,
    ANIM_HANDBAG_ITEM,
    ANIM_HAT_ITEM,

    // Hide
    ANIM_HIDE,

    // Effects
    ANIM_ENEMY_DEATH,
    ANIM_HEART_FULL,
    ANIM_HEART_BROKEN,

    // Ladders
    ANIM_LADDER_WHITE,
    ANIM_LADDER_BLUE,
    ANIM_LADDER_YELLOW,

    // Platforms
    ANIM_PLATFORM_RED,
    ANIM_PLATFORM_YELLOW,
    ANIM_PLATFORM_CONVEYER_BELT,
} animation_state_t;

typedef struct {
    entities_t type;
    animation_state_t anim_key;
    int start_x;
    int start_y;
    int frame_count;
    float frame_duration;
    int frame_width, frame_height;
} animation_sequence_t;

typedef struct {
    cairo_surface_t **frames;
    cairo_surface_t *current_frame;
    animation_state_t current_animation;
    int current_animation_index;
    int current_frame_index;
    float frame_time;
} animation_t;

typedef struct {
    entities_t type;
    cairo_surface_t *sprite_sheet;
} spritesheet_t;

typedef struct {
    animation_state_t anim_key;
    cairo_surface_t **frames;
    float frame_count;
} animation_frames_t;

/* 
* Abstract 'Classes'
*/
typedef struct {
    entities_t type;
    
    float previous_y;
    float x, y;
    float velocity_x, velocity_y;
    int direction;
    bool is_grounded;
    float width, height;

    animation_t animation;
} entity_t;

typedef struct {
    entity_t base;
    bool has_physics;
    bool is_cutscene_entity;
    int cutscene_id;
} geometry_t;

typedef struct {
    entity_t base;
} static_entity_t;

typedef struct {
    entities_t type;
    float x, y;
    float width, height;
    int direction;
    float spawn_interval;
    float spawn_timer;
} enemy_spawn_t;

/* 
* Level related
*/

typedef struct {
    entity_t base;
    float fly_time;
    bool jumping;
    bool on_ladder;
    int climb_direction;
} enemy_t;

typedef struct {
    entity_t base;
} effect_t;

typedef struct {
    entity_t base;
    float x, y;
    int points;
} item_t;


// Actors
typedef struct {
    entity_t base;
    float anim_interval;
    float anim_time;
} peach_t;

typedef struct {
    entity_t base;
    bool throw;
    bool has_thrown_this_cicle;
} donkey_kong_t;

typedef struct {
    entity_t base;
    bool is_dead;

    // Hammer
    bool has_hammer;
    float hammer_time;

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
    geometry_t *platforms;
    unsigned int num_platforms;

    // Ladders
    geometry_t *ladders;
    unsigned int num_ladders;

    // Static objects
    static_entity_t *static_entities;
    unsigned int num_static_entities;

    // Enemys
    enemy_t *enemies;
    unsigned int enemy_capacity;
    unsigned int num_enemies;

    enemy_spawn_t *enemy_spawns;
    unsigned int num_enemy_spawns;

    // Items
    item_t *items;
    unsigned int num_items;

    // Effects
    effect_t *effects;
    unsigned int num_effects;
    unsigned int effects_capacity;
} level_t;


/* 
* Game State
*/
typedef struct {
    // Input
    int *pressed_keys;
    unsigned int num_pressed_keys;

    // Level data
    level_t level;
    int current_level;
    int player_score;

    // Game mode
    game_mode_t mode;
    float key_cooldown;

    // Cutscene
    int current_cutscene;
    float cutscene_time;
    int cutscene_step;
} game_state_t;
