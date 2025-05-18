#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include "consts.h"
#include "core/gui.h"
#include "core/sprite/sprite_utils.h"
#include "entities/abstract/entity.h"

void gui_init(game_state_t *game_state);
void gui_draw(cairo_t *cr, game_state_t *game_state);
void gui_update(game_state_t *game_state, float dt_seconds);
void gui_player_lives(cairo_t *cr, game_state_t *game_state, float x, float y, float spacing);
void gui_bonus_points_sprite(cairo_t *cr, game_state_t *game_state, float x, float y);
void gui_text(cairo_t *cr, const char *string, float x, float y, float spacing, tint_t *tint);
void map_char_to_sprite(cairo_t *cr, char c, float x, float y, tint_t *tint);
void sprite_draw(cairo_t *cr, const entity_t *base, tint_t *tint);

static entity_t gui_char_entity;
static tint_t no_tint;
static tint_t red;
static tint_t light_blue;
static tint_t blue;

void gui_init(game_state_t *game_state) {
    game_state->bonus_points_timer = DECREMENT_TIMER;

    gui_char_entity.direction = 1;
    gui_char_entity.type = STATIC_ENTITY;
    gui_char_entity.animation.current_frame = NULL;
    gui_char_entity.animation.current_frame_index = 0;
    gui_char_entity.animation.frame_time = 0.0f;
    gui_char_entity.animation.frames = NULL;

    // no tint
    no_tint.tint_red = 1.0f;
    no_tint.tint_green = 1.0f;
    no_tint.tint_blue = 1.0f;
    no_tint.tint_alpha = 1.0f;

    // red
    red.tint_red = 1.0f;
    red.tint_green = 0.0f;
    red.tint_blue = 0.0f;
    red.tint_alpha = 1.0f;

    // blue
    blue.tint_red = 0.0f;
    blue.tint_green = 0.0f;
    blue.tint_blue = 1.0f;
    blue.tint_alpha = 1.0f;

    // red 99, green 231, blue 255
    light_blue.tint_red   = 0.388f;
    light_blue.tint_green = 0.906f;
    light_blue.tint_blue  = 1.0f;
    light_blue.tint_alpha = 1.0f;
}

void gui_draw(cairo_t *cr, game_state_t *game_state) {
    // Player Score
    char player_score_buffer[LETTER_BUFFER_SIZE];
    snprintf(player_score_buffer, sizeof(player_score_buffer), "%06d", game_state->player_score);
    gui_text(cr, player_score_buffer, PLAYER_SCORE_X, PLAYER_SCORE_Y, LETTER_SPACING, &no_tint);

    // Bounus Points
    char bonus_points_buffer[LETTER_BUFFER_SIZE];
    snprintf(bonus_points_buffer, sizeof(bonus_points_buffer), "%04d", game_state->bonus_points);
    gui_text(cr, bonus_points_buffer, BONUS_POINT_X, BONUS_POINT_Y, LETTER_SPACING, &light_blue);
    gui_bonus_points_sprite(cr, game_state, BONUS_POINT_FRAME_X, BONUS_POINT_FRAME_Y);

    // Current Level
    char level_buffer[12];
    snprintf(level_buffer, sizeof(level_buffer), "L=%02d", game_state->current_level);
    gui_text(cr, level_buffer, BONUS_POINT_FRAME_X, CURRENT_LEVEL_Y, LETTER_SPACING, &blue);

    gui_player_lives(cr, game_state, PLAYER_SCORE_X, PLAYER_LIVES_Y, LETTER_SPACING);
}

void gui_update(game_state_t *game_state, float dt_seconds) {
    game_state->bonus_points_timer -= dt_seconds;

    if (game_state->bonus_points_timer <= 0) {
        game_state->bonus_points -= BONUS_POINT_DECREMENT;
        game_state->bonus_points_timer = DECREMENT_TIMER;
    }
}

void gui_player_lives(cairo_t *cr, game_state_t *game_state, float x, float y, float spacing) {
    for (int i = 0; i < game_state->player_lives; i++) {
        gui_char_entity.x = x + i * spacing;
        gui_char_entity.y = y;
        gui_char_entity.animation.current_animation_index = -1;
        gui_char_entity.animation.current_frame_index = 0;
        gui_char_entity.animation.current_animation = ANIM_LIVES;
        set_animation_frames(&gui_char_entity);
        entity_draw(cr, &gui_char_entity);
    }
}

void gui_bonus_points_sprite(cairo_t *cr, game_state_t *game_state, float x, float y) {
    gui_char_entity.x = x;
    gui_char_entity.y = y;
    gui_char_entity.animation.current_animation_index = -1;
    gui_char_entity.animation.current_frame_index = 0;
    gui_char_entity.animation.current_animation = ANIM_BONUS_POINTS;
    set_animation_frames(&gui_char_entity);
    entity_draw(cr, &gui_char_entity);
}

void gui_text(cairo_t *cr, const char *string, float x, float y, float spacing, tint_t *tint) {
    for (int i = 0; i < strlen(string); i++) {
        map_char_to_sprite(cr, string[i], x + i * spacing, y, tint);
    }
}

void map_char_to_sprite(cairo_t *cr, char c, float x, float y, tint_t *tint) {
    gui_char_entity.x = x;
    gui_char_entity.y = y;

    if (c >= '0' && c <= '9') {
        // map correct char by subtracting '0' char from given char
        gui_char_entity.animation.current_animation = ANIM_NUMBERS;
        gui_char_entity.animation.current_frame_index = c - '0';
    }
    else if (c >= 'A' && c <= 'Z') {
        // map correct char by subtracting '0' char from given char
        gui_char_entity.animation.current_animation = ANIM_ALPHABET;
        gui_char_entity.animation.current_frame_index = c - 'A';
    }
    else if (strchr(".-?:=()", c)) {
        gui_char_entity.animation.current_animation = ANIM_SPECIAL;

        switch (c) {
            case '.': gui_char_entity.animation.current_frame_index = 0; break;
            case '-': gui_char_entity.animation.current_frame_index = 1; break;
            case '?': gui_char_entity.animation.current_frame_index = 2; break;
            case ':': gui_char_entity.animation.current_frame_index = 3; break;
            case '=': gui_char_entity.animation.current_frame_index = 4; break;
            case '(': gui_char_entity.animation.current_frame_index = 5; break;
            case ')': gui_char_entity.animation.current_frame_index = 6; break;
        }
    }
    else {
        gui_char_entity.animation.current_animation = ANIM_HIDE;
    }
            
    // reset cache and force lookup
    gui_char_entity.animation.current_animation_index = -1;

    set_animation_frames(&gui_char_entity);
    sprite_draw(cr, &gui_char_entity, tint);
}

void sprite_draw(cairo_t *cr, const entity_t *base, tint_t *tint) {
    cairo_save(cr);

    cairo_translate(cr, base->x, base->y);
    cairo_scale(cr, SCALE, SCALE);

    cairo_surface_t *frame_surface = base->animation.current_frame;

    if (frame_surface != NULL) {
        cairo_set_source_rgba(cr, tint->tint_red, tint->tint_green, tint->tint_blue, tint->tint_alpha);

        cairo_pattern_t *mask_pattern = cairo_pattern_create_for_surface(frame_surface);
        cairo_pattern_set_filter(mask_pattern, CAIRO_FILTER_NEAREST);

        cairo_mask(cr, mask_pattern);

        cairo_pattern_destroy(mask_pattern);

    } else if (base->animation.current_animation == ANIM_HIDE) {}
    cairo_restore(cr);
}