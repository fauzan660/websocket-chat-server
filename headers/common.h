#pragma once
#include <unistd.h>

#define MAX_CONNECTIONS 5
#define MAX_CLIENTS 20
#define BUF_SIZE 4096
#define WS_BUF_SIZE 4096
#define MAGIC_WEBSOCKET_UUID_STRING "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
#define WS_ENDPOINT "/websocket"

typedef struct {
  int fd;
} Client;

void close_socket(Client *c);
