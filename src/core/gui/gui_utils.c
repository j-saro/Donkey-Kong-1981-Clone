#include <stdio.h>
#include <string.h>
#include "consts.h"
#include "core/gui/gui_utils.h"
#include "core/sprite/sprite_utils.h"

float calc_center_text(const char *text, float letter_spacing);
void gui_text(cairo_t *cr, entity_t *entity, const char *string, float x, float y, float spacing, tint_t *tint);
void map_char_to_sprite(cairo_t *cr, entity_t *entity, char c, float x, float y, tint_t *tint);
void sprite_draw(cairo_t *cr, const entity_t *base, tint_t *tint);
void draw_rectangle(cairo_t *cr, float pos_x, float pos_y, float width, float height);

float calc_center_text(const char *text, float letter_spacing) {
    int text_len = strlen(text);
    float text_pixel_width = (float)text_len * letter_spacing;
    float x_pos = (BASE_WIDTH / 2.0f) - (text_pixel_width / 2.0f);
    return x_pos;
}

void gui_text(cairo_t *cr, entity_t *entity, const char *string, float x, float y, float spacing, tint_t *tint) {
    for (int i = 0; i < strlen(string); i++) {
        map_char_to_sprite(cr, entity, string[i], x + i * spacing, y, tint);
    }
}

void map_char_to_sprite(cairo_t *cr, entity_t *entity, char c, float x, float y, tint_t *tint) {
    entity->x = x;
    entity->y = y;

    if (c >= '0' && c <= '9') {
        // map correct char by subtracting '0' char from given char
        entity->animation.current_animation = ANIM_NUMBERS;
        entity->animation.current_frame_index = c - '0';
    }
    else if (c >= 'A' && c <= 'Z') {
        // map correct char by subtracting '0' char from given char
        entity->animation.current_animation = ANIM_ALPHABET;
        entity->animation.current_frame_index = c - 'A';
    }
    else if (strchr(".-?:=()", c)) {
        entity->animation.current_animation = ANIM_SPECIAL;

        switch (c) {
            case '.': entity->animation.current_frame_index = 0; break;
            case '-': entity->animation.current_frame_index = 1; break;
            case '?': entity->animation.current_frame_index = 2; break;
            case ':': entity->animation.current_frame_index = 3; break;
            case '=': entity->animation.current_frame_index = 4; break;
            case '(': entity->animation.current_frame_index = 5; break;
            case ')': entity->animation.current_frame_index = 6; break;
        }
    }
    else {
        entity->animation.current_animation = ANIM_HIDE;
    }
            
    // reset cache and force lookup
    entity->animation.current_animation_index = -1;

    set_animation_frames(entity);
    sprite_draw(cr, entity, tint);
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

void draw_rectangle(cairo_t *cr, float pos_x, float pos_y, float width, float height) {
    cairo_save(cr);

    cairo_set_source_rgb(cr, 0.0, 0.0, 0.5);
    cairo_rectangle(cr, pos_x, pos_y, width, height);
    cairo_fill(cr);

    cairo_restore(cr);
}