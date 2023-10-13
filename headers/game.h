#ifndef GAME_H_
#define GAME_H_

#include "raylib.h"

#ifndef TICK_NUMBER
#define TICK_NUMBER 144
#endif

float Get_delta_time(void) { return GetFrameTime(); }
Vector2 Apply_force(float mass, Vector2 curr_pos, Vector2 velocity,
                    float acceleration, float dt) {

  curr_pos.x += velocity.x * dt;
  curr_pos.y += velocity.y * dt;
  return curr_pos;
}

#endif
