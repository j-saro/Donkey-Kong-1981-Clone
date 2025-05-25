#pragma once

#include <cairo.h>
#include <stdbool.h>
#include "consts.h"

typedef enum {
    GAME_MODE_MENU,
    GAME_MODE_NORMAL,
    GAME_MODE_CUTSCENE,
    GAME_MODE_PAUSED,
    GAME_MODE_EFFECT,
    GAME_MODE_GAME_OVER,
    GAME_MODE_GAME_FINISH,
} game_mode_t;

typedef enum {
    CUTSCENE_DK_INTRO,
    CUTSCENE_DK_ESCAPE,
    CUTSCENE_DK_DEATH,
    CUTSCENE_MARIO_DEATH,
} cutscene_t;

typedef enum {
    #define X(name) name,
        ENTITY_LIST
    #undef X
} entities_t;

/* 
* Animation related
*/
typedef enum {
    #define X(name) name,
        ANIM_LIST
    #undef X
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

typedef struct {
    float tint_red, tint_green, tint_blue, tint_alpha;
} tint_t;

/* 
* Abstract 'Classes'
*/
typedef struct {
    entities_t type;
    
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
} geometry_t;

typedef struct {
    entity_t base;
} static_entity_t;

typedef struct {
    entities_t type;
    animation_state_t anim_state; 
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
    float stop_x_threshold;
} enemy_t;

typedef struct {
    entity_t base;
    bool once;
} effect_t;

typedef struct {
    entity_t base;
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
    float previous_y;
    float flight_timer;
    bool on_elevator;
    float spawn_x, spawn_y;
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

    // finish line
    float finish_line;
    // counts up to 1.0f
    float frame_timer;
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
    bool bonus_live;
    int player_lives;
    int player_score;
    int bonus_points;
    float bonus_points_timer;
    int pressed_buttons;
    player_t player_clone;

    // Game mode
    game_mode_t mode;
    float key_cooldown;

    // Cutscene
    cutscene_t current_cutscene;
    float cutscene_time;
    int cutscene_step;
} game_state_t;
