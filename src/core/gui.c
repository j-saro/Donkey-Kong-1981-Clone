#include <gtk/gtk.h>
#include "consts.h"
#include "core/gui.h"
#include "core/gui_utils.h"
#include "core/sprite/sprite_utils.h"
#include "entities/abstract/entity.h"

void gui_init(game_state_t *game_state);
void gui_startmenu_draw(cairo_t *cr, game_state_t *game_state);
void gui_game_complete_draw(cairo_t *cr, game_state_t *game_state);
void gui_game_over_draw(cairo_t *cr, game_state_t *game_state);
void gui_game_draw(cairo_t *cr, game_state_t *game_state);
void gui_update(game_state_t *game_state, float dt_seconds);
void gui_player_lives(cairo_t *cr, game_state_t *game_state, float x, float y, float spacing);
void gui_bonus_points_sprite(cairo_t *cr, game_state_t *game_state, float x, float y);

static entity_t gui_char_entity;

// font color
static tint_t no_tint = {1.0, 1.0, 1.0, 1.0};
static tint_t red = {1.0, 0.0, 0.0, 1.0};
static tint_t blue = {0.0, 0.0, 1.0, 1.0};
static tint_t light_blue = {0.388f, 0.906f, 1.0, 1.0}; // red 99, green 231, blue 255

static const char *start_text = GUI_TEXT_START;
static const char *game_over_text = GUI_TEXT_GAME_OVER;
static const char *return_text = GUI_TEXT_RETURN;

void gui_init(game_state_t *game_state) {
    gui_char_entity.direction = 1;
    gui_char_entity.type = STATIC_ENTITY;
    gui_char_entity.animation.current_frame = NULL;
    gui_char_entity.animation.current_frame_index = 0;
    gui_char_entity.animation.frame_time = 0.0f;
    gui_char_entity.animation.frames = NULL;
}

void gui_startmenu_draw(cairo_t *cr, game_state_t *game_state) {
    float pos_x = calc_center_text(start_text, LETTER_SPACING);
    gui_text(cr, &gui_char_entity, start_text, pos_x, MENU_TEXT, LETTER_SPACING, &no_tint);
}

void gui_game_complete_draw(cairo_t *cr, game_state_t *game_state) {
    float pos_x = calc_center_text(return_text, LETTER_SPACING);
    int text_len = strlen(return_text);
    float box_width = (float)text_len * LETTER_SPACING + 2 * SPACING;
    draw_rectangle(cr, pos_x - SPACING, COMPLETE_TEXT_Y - SPACING, box_width, BOX_HEIGHT);
    gui_text(cr, &gui_char_entity, return_text, pos_x, COMPLETE_TEXT_Y, LETTER_SPACING, &no_tint);
}

void gui_game_over_draw(cairo_t *cr, game_state_t *game_state) {
    float box_width;
    int text_len;
    float pos_x;

    text_len = strlen(game_over_text);
    box_width = (float)text_len * LETTER_SPACING + 2 * SPACING;
    pos_x = calc_center_text(game_over_text, LETTER_SPACING);
    draw_rectangle(cr, pos_x - SPACING, GAME_OVER_TEXT_Y - SPACING, box_width, BOX_HEIGHT);
    gui_text(cr, &gui_char_entity, game_over_text, pos_x, GAME_OVER_TEXT_Y, LETTER_SPACING, &red);

    text_len = strlen(return_text);
    box_width = (float)text_len * LETTER_SPACING + 2 * SPACING;
    pos_x = calc_center_text(return_text, LETTER_SPACING);
    draw_rectangle(cr, pos_x - SPACING, GAME_OVER_RETURN_TEXT_Y - SPACING, box_width, BOX_HEIGHT);
    gui_text(cr, &gui_char_entity, return_text, pos_x, GAME_OVER_RETURN_TEXT_Y, LETTER_SPACING, &no_tint);
}

void gui_game_draw(cairo_t *cr, game_state_t *game_state) {
    char buffer[LETTER_BUFFER_SIZE];

    // Player Score
    snprintf(buffer, sizeof(buffer), "%06d", game_state->player_score);
    gui_text(cr, &gui_char_entity, buffer, PLAYER_SCORE_X, PLAYER_SCORE_Y, LETTER_SPACING, &no_tint);

    // Bounus Points
    snprintf(buffer, sizeof(buffer), "%04d", game_state->bonus_points);
    gui_text(cr, &gui_char_entity, buffer, BONUS_POINT_X, BONUS_POINT_Y, LETTER_SPACING, &light_blue);
    gui_bonus_points_sprite(cr, game_state, BONUS_POINT_FRAME_X, BONUS_POINT_FRAME_Y);

    // Current Level
    snprintf(buffer, sizeof(buffer), "L=%02d", game_state->current_level);
    gui_text(cr, &gui_char_entity, buffer, BONUS_POINT_FRAME_X, CURRENT_LEVEL_Y, LETTER_SPACING, &blue);

    gui_player_lives(cr, game_state, PLAYER_SCORE_X, PLAYER_LIVES_Y, LETTER_SPACING);
}

void gui_update(game_state_t *game_state, float dt_seconds) {
    game_state->bonus_points_timer -= dt_seconds;

    if (game_state->bonus_points_timer <= 0 && game_state->bonus_points > 0) {
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