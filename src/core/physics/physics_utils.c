#include "core/physics/physics_utils.h"

bool player_object_collision(player_t *player, entity_t *base, float object_width, float object_heigth);
void entity_jump(entity_t *base, float jump_force);

// checks player object collision
bool player_object_collision(player_t *player, entity_t *base, float object_width, float object_heigth) {
    float epsilon = 2.0f; // shrinks hit box

    float player_left = player->base.x + epsilon;
    float player_right = player->base.x + player->base.width - epsilon;
    float player_top = player->base.y - epsilon;
    float player_bottom = player->base.y + player->base.height + epsilon;

    float object_left = base->x + epsilon;
    float object_right = base->x + object_width - epsilon;
    float object_top = base->y;
    float object_bottom = base->y + object_heigth;

    return !(player_right < object_left ||
             player_left > object_right ||
             player_bottom < object_top ||
             player_top > object_bottom);
}

void entity_jump(entity_t *base, float jump_force) {
    if (base->is_grounded) {
        base->velocity_y = -jump_force;
        base->is_grounded = false;
    }
}