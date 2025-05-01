#include "core/physics/physics_utils.h"

bool player_object_collision(player_t *player, entity_t *base, float object_width, float object_heigth);

bool player_object_collision(player_t *player, entity_t *base, float object_width, float object_heigth) {
    float player_left = player->base.x;
    float player_right = player->base.x + player->base.width;
    float player_top = player->base.y;
    float player_bottom = player->base.y + player->base.height;

    float object_left = base->x;
    float object_right = base->x + object_width;
    float object_top = base->y;
    float object_bottom = base->y + object_heigth;

    return !(player_right < object_left ||
             player_left > object_right ||
             player_bottom < object_top ||
             player_top > object_bottom);
}