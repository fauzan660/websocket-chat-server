#include "../headers/ws.h"
#include "../headers/action_handler.h"
#include "WS_Frame/headers/frame-send.h"
#include "WS_Frame/headers/frame.h"
#include "headers/ws_helpers.h"
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <errno.h>
#include <nlohmann/json.hpp>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <sys/socket.h>
#include <vector>
using namespace std;
using json = nlohmann::json;

bool check_request_ws(string method, string target, string http_version,
                      map<string, string> headers_map) {
  if (method != "GET")
    return false;
  string ver = http_version.substr(http_version.find('/') + 1);
  if (stof(ver) < 1.1f)
    return false;
  auto upg = headers_map.find("upgrade");
  if (upg == headers_map.end() || upg->second != "websocket")
    return false;
  auto con = headers_map.find("connection");
  if (con == headers_map.end() || con->second != "upgrade")
    return false;
  if (headers_map.find("sec-websocket-key") == headers_map.end())
    return false;
  return true;
}

int handle_request_ws(Client *c, map<string, string> headers_map,
                      Client clients[]) {

  auto key = headers_map.find("sec-websocket-key");
  if (key == headers_map.end())
    return -1;

  string ws_response_key = generate_websocket_response_key(key->second);
  if (ws_response_key.empty())
    return -1;

  string ws_response = "HTTP/1.1 101 Switching Protocols\r\n"
                       "Upgrade: websocket\r\n"
                       "Connection: Upgrade\r\n"
                       "Sec-WebSocket-Accept: " +
                       ws_response_key + "\r\n\r\n";

  int send_status = ::send(c->fd, ws_response.c_str(), ws_response.size(), 0);
  if (send_status < 0) {
    perror("send");
    return -1;
  }

  for (int j = 0; j < MAX_CLIENTS; j++) {
    if (clients[j].fd == c->fd) {
      clients[j].fd = -1;
      break;
    }
  }
  return send_status;
}

// void handle_websocket_client(Client *ws_c, Client clients[],
//                              Client websocket_clients[]) {
//
//   static uint8_t ws_buf[WS_BUF_SIZE]; // static = local to this translation
//   unit
//
//   int ws_recv_status = ::recv(ws_c->fd, ws_buf, WS_BUF_SIZE - 1, 0);
//   if (ws_recv_status == 0) {
//     close_socket(ws_c);
//     return;
//   }
//   if (ws_recv_status < 0) {
//     if (errno == EAGAIN || errno == EWOULDBLOCK)
//       return;
//     close_socket(ws_c);
//     return;
//   }
//   parse_websocket_request(ws_buf, ws_recv_status);
//
//   // Action action = get_client_action(json_req_object);
//   // handle_action(action, json_req_object);
//
//   WS_Frame_Client frame_client;
//   char client_str[] =
//       "<h1>Hello from server</h1><p>WebSocket connection works</p>";
//   frame_client.parse_response(client_str);
//   vector<uint8_t> bytes_response = frame_client.parse_bytes();
//
//   int ws_send_status =
//       ::send(ws_c->fd, bytes_response.data(), bytes_response.size(), 0);
//   if (ws_send_status <= 0) {
//     close_socket(ws_c);
//     return;
//   }
//   printf("successfully sent data to ws %d. \n", ws_c->fd);
// }
#include <iostream>
