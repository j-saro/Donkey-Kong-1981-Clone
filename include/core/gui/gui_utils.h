#pragma once

#include "types.h"
#include <gtk/gtk.h>

float calc_center_text(const char *text, float letter_spacing);
void gui_text(cairo_t *cr, entity_t *entity, const char *string, float x, float y, float spacing, tint_t *tint);
void draw_rectangle(cairo_t *cr, float pos_x, float pos_y, float width, float height);
