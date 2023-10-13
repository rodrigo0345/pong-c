#ifndef PLAYER_H_
#define PLAYER_H_

#include "game.h"
#include "raylib.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef MALLOC
#define MALLOC(type, size) (type *)malloc(sizeof(type) * size)
#endif

typedef struct Player {
  int id;
  int score;
  Vector2 position;
  Vector2 size;
  Color color;
} Player;

typedef struct Ball {
  Vector2 position;
  Vector2 size;
  Vector2 velocity;
  Color color;
} Ball;

Ball *New_ball(Vector2 position, Vector2 initial_vel, Vector2 size, Color c) {
  Ball *ball = MALLOC(Ball, 1);
  ball->position = position;
  ball->color = c;
  ball->size = size;
  ball->velocity = initial_vel;

  return ball;
}

Player *New_player(int id, int score, Vector2 position, Vector2 size, Color c) {
  Player *player = MALLOC(Player, 1);
  player->id = id;
  player->score = score;
  player->position = position;
  player->color = c;
  player->size = size;

  return player;
}

enum COLLIDER { FLOOR, CEILING, WALL_RIGHT, WALL_LEFT, NONE };

// return 0 if false, 1 if top ceiling, -1 if floor
enum COLLIDER Is_ball_colliding(Ball *player, int screen_width,
                                int screen_height) {
  enum COLLIDER aux =
      player->position.y < 0
          ? CEILING
          : ((player->position.y + player->size.y > screen_height) ? FLOOR
                                                                   : NONE);
  if (aux != NONE)
    return aux;

  return player->position.x < 0                               ? WALL_LEFT
         : player->position.x + player->size.x > screen_width ? WALL_RIGHT
                                                              : NONE;
}
enum COLLIDER Is_player_colliding(Player *player, int screen_width,
                                  int screen_height) {
  enum COLLIDER aux =
      player->position.y < 0
          ? CEILING
          : ((player->position.y + player->size.y > screen_height) ? FLOOR
                                                                   : NONE);
  if (aux != NONE)
    return aux;

  return player->position.x < 0                               ? WALL_LEFT
         : player->position.x + player->size.x > screen_width ? WALL_RIGHT
                                                              : NONE;
}

void Draw_player(Player *player) {
  Rectangle rect = {player->position.x, player->position.y, player->size.x,
                    player->size.y};
  DrawRectangleRec(rect, player->color);
}

void Draw_ball(Ball *player) {
  DrawCircle(player->position.x, player->position.y, player->size.x,
             player->color);
}

void Destroy_player(Player *player) {
  assert(player != NULL);
  free(player);
}

void Move_player(Player *player, Vector2 velocity, float dt) {
  assert(player != NULL);

  player->position = Apply_force(1, player->position, velocity, 1, dt);

  // make sure he is inside the screen
  enum COLLIDER collider =
      Is_player_colliding(player, GetScreenWidth(), GetScreenHeight());

  switch (collider) {
  case FLOOR:
    player->position.y = GetScreenHeight() - player->size.y;
    break;
  case CEILING:
    player->position.y = 0;
    break;
  case WALL_LEFT:
    player->position.x = 0;
    break;
  case WALL_RIGHT:
    player->position.x = GetScreenWidth() - player->size.x;
    break;
  }
}

char Is_player_touching_ball(Ball *ball, Player *player) {
  // Check if the ball is touching the player using correct conditions
  return (
      ball->position.x + ball->size.x >= player->position.x &&
      ball->position.x - ball->size.x <= player->position.x + player->size.x &&
      ball->position.y + ball->size.y >= player->position.y &&
      ball->position.y - ball->size.y <= player->position.y + player->size.y);
}

// if the ball collides during the movement the game can do its thing and give
// point to the appropiate player
enum COLLIDER Move_ball(Ball *ball, float dt, Player **players,
                        size_t num_player) {
  assert(ball != NULL);

  ball->position = Apply_force(1, ball->position, ball->velocity, 1, dt);

  for (int i = 0; i < num_player; i++) {
    if (Is_player_touching_ball(ball, players[i])) {
      ball->velocity.x *= -1 * 1.05;
      int adjustment = (ball->position.x > (int)(GetScreenWidth() / 2))
                           ? -players[1]->size.x
                           : players[1]->size.x;
      ball->position.x += adjustment;
    }
  }

  // make sure he is inside the screen
  enum COLLIDER collider =
      Is_ball_colliding(ball, GetScreenWidth(), GetScreenHeight());

  switch (collider) {
  case FLOOR:
    ball->position.y = GetScreenHeight() - ball->size.y;
    ball->velocity.y *= -1;
    break;
  case CEILING:
    ball->position.y = 0;
    ball->velocity.y *= -1;
    break;
    break;
  case WALL_LEFT:
    ball->position.x = 0;
    break;
  case WALL_RIGHT:
    ball->position.x = GetScreenWidth() - ball->size.x;
    break;
  }

  return collider;
}

#endif
