#pragma once

#include "types.h"
#include "cJSON.h"

void entity_parse(entity_t *base, cJSON *json);
void entity_draw(cairo_t *cr, const entity_t *base);