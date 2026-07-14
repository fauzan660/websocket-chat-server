#pragma once
#include "common.h"
#include <map>
#include <string>

enum class Action {
  CREATE_USER,
  CREATE_ROOM,
  VIEW_ROOM_SUGGESTIONS,
  ADD_SUGGESTION,
  ACCEPT_SUGGESTION,
  UNKNOWN
};

bool check_request_ws(std::string method, std::string target,
                      std::string http_version,
                      std::map<std::string, std::string> headers_map);

int handle_request_ws(Client *c, std::map<std::string, std::string> headers_map,
                      Client clients[]);

void handle_websocket_client(Client *ws_c, Client clients[],
                             Client websocket_clients[]);

Action parse_websocket_request_new(uint8_t websocket_buffer[],
                                   int request_size);
