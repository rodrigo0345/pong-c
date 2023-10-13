#include "../headers/server.h"
#include "../headers/hashmap.h"
#include "../headers/thunder_network.h"
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct Player {
  char *ip;
  char *x;
  char *y;
};

#define NUMBER_PLAYERS 2
#define PORT 8000

struct ThreadArgument {
  int conn;
  struct hashmap *hashmap;
};

// "x,y"
char **positions = NULL;

void *handle_request(void *a) {
  struct ThreadArgument *arg = (struct ThreadArgument *)a;

  char *msg = malloc(sizeof(char) * 100);
  Thunder_Read_message(arg->conn, msg, 100);

  switch (msg[0]) {
  case 'r':
    printf(""); // this is not doing nothing..
    void *item;
    size_t iter = 0;
    char *final_string = malloc(sizeof(char) * 100);
    while (hashmap_iter(arg->hashmap, &iter, &item)) {
      sprintf(final_string, "%s,%s,%s\n", ((struct Player *)item)->ip,
              ((struct Player *)item)->x, ((struct Player *)item)->y);
    }
    Thunder_Server_send(arg->conn, msg, strlen(msg));
    break;

    // "u,x,y,ip"
  case 'u':
    strtok(msg, ",");

    // this needs a lot more checks
    struct Player *player = malloc(sizeof(struct Player));
    player->x = strtok(NULL, ",");
    player->y = strtok(NULL, ",");
    player->ip = strtok(NULL, ",");

    hashmap_set(arg->hashmap, player);
    break;
  default:
    // send not valid message
    Thunder_Server_send(arg->conn, "nv", strlen("nv"));
  }

end:
  free(arg);
}

int message_compare(const void *a, const void *b, void *udata) {
  const struct Player *ua = a;
  const struct Player *ub = b;
  return strcmp(ua->ip, ub->ip);
}

uint64_t message_hash(const void *item, uint64_t seed0, uint64_t seed1) {
  const struct Player *player = item;
  return hashmap_sip(player->ip, strlen(player->ip), seed0, seed1);
}

void server_start() {

  struct hashmap *hashmap =
      hashmap_new(sizeof(struct Player), 2, 0, 0, message_hash, message_compare,
                  NULL, NULL);

  Server *server = Thunder_New_server(PORT);
  while (true) {
    // arg is freed in the handle request
    printf("Awaiting connection\n");
    struct ThreadArgument *arg = malloc(sizeof(struct ThreadArgument));
    arg->hashmap = hashmap;
    int conn = Thunder_Await_connection(server);
    pthread_t t;
    arg->conn = conn;
    pthread_create(&t, NULL, handle_request, (void *)arg);
    Thunder_Close_connection(conn);
  }

  Thunder_Close_server(server);
  hashmap_free(hashmap);
}
