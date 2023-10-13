#include "../headers/game.h"
#include "../headers/player.h"
#include "raylib.h"
#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>

#define USE_SERVER false

struct ThreadArg {
  Player *p;
  float dt;
  KeyboardKey keyUp;
  KeyboardKey keyDown;
};

void *process_move(void *input) {
  struct ThreadArg *arg = (struct ThreadArg *)input;

  if (IsKeyDown(arg->keyUp)) {
    Move_player(arg->p, (Vector2){0, -40}, arg->dt);
  } else if (IsKeyDown(arg->keyDown)) {
    Move_player(arg->p, (Vector2){0, 40}, arg->dt);
  }

  // Client *client = new_client(8000, "localhost");
  // client_connect(client);

  // char *msg = malloc(sizeof(char) * 100);

  // // get my ip
  // sprintf(msg, "u,%f,%f,%s", arg->p->position.x, arg->p->position.y,
  //         Thunder_Get_computer_ip());
  // client_send(client, msg, strlen(msg));
  // // update the server with the new player pos
}

extern void start_game() {

  InitWindow(800, 600, "Pong game");

  // start the two players
  Color cp1 = {0, 0, 0, 255};
  Color cp2 = {255, 0, 0, 255};
  Color ball_color = {233, 210, 0, 255};
  ;

  Player *p1 = New_player(0, 0, (Vector2){50, 300}, (Vector2){10, 100}, cp1);

  Player *p2 = New_player(1, 0, (Vector2){GetScreenWidth() - (50 + 50), 300},
                          (Vector2){10, 100}, cp2);

  int ball_x_vel = GetRandomValue(-40, 40) % 2 == 0 ? 80 : -80;
  int ball_y_vel = GetRandomValue(-80, 80);

  Ball *ball = New_ball((Vector2){400, 300}, (Vector2){ball_x_vel, ball_y_vel},
                        (Vector2){20, 20}, ball_color);

  Player *players[] = {p1, p2}; // Assuming a maximum of two players

  pthread_t thread;

  while (WindowShouldClose() == false) {
    ClearBackground(RAYWHITE);
    float dt = 10 * Get_delta_time();
    struct ThreadArg *arg = malloc(sizeof(struct ThreadArg));
    arg->p = p1;
    arg->dt = dt;
    arg->keyUp = KEY_W;
    arg->keyDown = KEY_S;
    pthread_t thread1; // Create a new thread variable
    pthread_create(&thread1, NULL, process_move, arg);

    pthread_t thread2; // Create a new thread variable
    if (!USE_SERVER) {
      struct ThreadArg *arg2 = malloc(sizeof(struct ThreadArg));
      arg2->dt = dt;
      arg2->p = p2;
      arg2->keyUp = KEY_UP;
      arg2->keyDown = KEY_DOWN;
      pthread_create(&thread2, NULL, process_move, arg2);
    }

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    enum COLLIDER collider =
        Move_ball(ball, dt, players, sizeof(players) / sizeof(players[0]));

    BeginDrawing();
    for (int i = 0; i < 2; i++) {
      Player *p = players[i];
      Draw_player(p);
    }
    Draw_ball(ball);

    if (collider == WALL_RIGHT) {
      ball_x_vel = GetRandomValue(-40, 40) % 2 == 0 ? 40 : -40;
      ball_y_vel = GetRandomValue(-40, 40);

      p1->score++;
      ball->position = (Vector2){400, 300};
      ball->velocity = (Vector2){ball_x_vel, ball_y_vel};
    } else if (collider == WALL_LEFT) {
      ball_x_vel = GetRandomValue(-40, 40) % 2 == 0 ? 40 : -40;
      ball_y_vel = GetRandomValue(-40, 40);

      p2->score++;
      ball->position = (Vector2){400, 300};
      ball->velocity = (Vector2){ball_x_vel, ball_y_vel};
    }

    char *score_p = malloc(sizeof(char) * 10 + 1);
    sprintf(score_p, "Player1: %d", p1->score);

    DrawText(score_p, (GetScreenWidth() / 2) - 200, 10, 20, BLUE);

    sprintf(score_p, "Player2: %d", p2->score);

    DrawText(score_p, (GetScreenWidth() / 2) + 200, 10, 20, RED);

    DrawFPS(10, 10);
    EndDrawing();
  }
}

void draw_fps() {
  BeginDrawing();
  DrawFPS(10, 10);
  EndDrawing();
}
