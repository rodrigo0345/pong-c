#ifndef THUNDER_NETWORK_H_
#define THUNDER_NETWORK_H_
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* for strncpy */
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// Server options

#ifndef DEFAULT_PORT
#define DEFAULT_PORT 9999
#endif

#ifndef MESSAGE_SIZE
#define MESSAGE_SIZE 1024 // 1024bytes
#endif

typedef struct Server {
  int socket_fd;
  struct sockaddr_in address;
} Server;

Server *Thunder_New_server(int port) {
  int server_fd = 0;

  // no clue what this opt is..
  int opt = 1;

  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Error creating a new socket!");
    exit(1);
  }
  // attach the socket to the port
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt))) {
    perror("Error binding the socket to port");
    exit(1);
  }

  struct sockaddr_in address = {.sin_family = AF_INET,
                                .sin_port = htons(port),
                                .sin_addr.s_addr = INADDR_ANY};

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("Error with binding");
    exit(1);
  }

  Server *s = (Server *)malloc(sizeof(Server));

  s->socket_fd = server_fd;
  s->address = address;

  return s;
}

void Thunder_Close_server(Server *server) {
  shutdown(server->socket_fd, SHUT_RDWR);
}

void Thunder_Close_connection(int conn) { close(conn); }

int Thunder_Await_connection(Server *server) {

  // 20 is the number of connections accepted before being
  // refused
  if (listen(server->socket_fd, 1) < 0) {
    perror("Error listening for new connections");
    exit(1);
  }

  int new_conn;
  int addrlen = sizeof(server->address);
  if ((new_conn = accept(server->socket_fd, (struct sockaddr *)&server->address,
                         (socklen_t *)&addrlen)) < 0) {
    perror("Error accepting the connection");
    exit(1);
  }

  return new_conn;
}

void Thunder_Read_message(int conn, void *message, size_t message_size) {
  read(conn, message, message_size);
}

void Thunder_Server_send(int conn, void *message, size_t message_size) {
  send(conn, message, message_size, 0);
}

// Client options

typedef struct Client {
  int client_fd;
  struct sockaddr_in address;
  char *addr_name;
} Client;

Client *new_client(int port, char *address) {

  int client_fd;
  if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("Error creating socket\n");
    exit(1);
  }

  struct sockaddr_in addr = {.sin_family = AF_INET, .sin_port = htons(port)};

  if (inet_pton(AF_INET, address, &addr.sin_addr) <= 0) {
    printf("Address: %s\n", address);
    perror("Error the provided address is invalid\n");
  }

  Client *c = (Client *)malloc(sizeof(Client));
  c->client_fd = client_fd;
  c->address = addr;
  c->addr_name = address;
  return c;
}

int client_connect(Client *client) {
  int status;
  if ((status = connect(client->client_fd, (struct sockaddr *)&client->address,
                        sizeof(client->address)))) {
    printf("Connection to %s:%d failed", client->addr_name,
           client->address.sin_port);
    return -1;
  }

  return status;
}

void client_send(Client *client, void *message, size_t message_size) {
  if (message_size >= 1024) {
    perror("Error the message you are trying to send is too long");
  }
  send(client->client_fd, message, message_size, 0);
}

void client_read(Client *client, void *message) {
  read(client->client_fd, message, MESSAGE_SIZE);
}

void client_close(Client *c) { close(c->client_fd); }

// crazy abstraction bc I am lazy :)
char *send_message(int port, char *address, char *message,
                   size_t message_size) {
  Client *me = new_client(port, address);
  if (client_connect(me) == -1) {
    return "";
  }
  client_send(me, message, message_size);
  client_read(me, message);
  return message;
}

// Returns hostname for the local computer
// void checkHostName(int hostname) {
//   if (hostname == -1) {
//     perror("gethostname");
//     exit(1);
//   }
// }

// // Returns host information corresponding to host name
// void checkHostEntry(struct hostent *hostentry) {
//   if (hostentry == NULL) {
//     perror("gethostbyname");
//     exit(1);
//   }
// }

// // Converts space-delimited IPv4 addresses
// // to dotted-decimal format
// void checkIPbuffer(char *IPbuffer) {
//   if (NULL == IPbuffer) {
//     perror("inet_ntoa");
//     exit(1);
//   }
// }

// char *Thunder_Get_computer_ip() {
//   char hostbuffer[256];
//   char *IPbuffer;
//   struct hostent *host_entry;
//   int hostname;

//   // To retrieve hostname
//   hostname = gethostname(hostbuffer, sizeof(hostbuffer));
//   checkHostName(hostname);

//   // To retrieve host information
//   host_entry = gethostbyname(hostbuffer);
//   checkHostEntry(host_entry);

//   // To convert an Internet network
//   // address into ASCII string
//   return IPbuffer = inet_ntoa(*((struct in_addr
//   *)host_entry->h_addr_list[0]));
// }

#endif
