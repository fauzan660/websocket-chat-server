#pragma once
#include "common.h"
#include <map>
#include <string>

bool check_request_ws(std::string method, std::string target,
                      std::string http_version,
                      std::map<std::string, std::string> headers_map);

int handle_request_ws(Client *c, std::map<std::string, std::string> headers_map,
                      Client clients[]);

void handle_websocket_client(Client *ws_c, Client clients[],
                             Client websocket_clients[]);
