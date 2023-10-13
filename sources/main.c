#include "../headers/server.h"
#include "raylib.h"
#include <stdio.h>

extern void start_game();

int main(int argc, char **argv) {
  if (argc == 3) {
    server_start();
  } else {
    start_game();
  }

  return 0;
}
