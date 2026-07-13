#pragma once
#include "common.h"
#include <map>
#include <string>
#include <sys/select.h>
using namespace std;

void parse_request(std::string message, std::string &method,
                   std::string &target, std::string &http_version,
                   std::map<std::string, std::string> &headers_map);

int handle_client(Client *c, char server_buf[], char response_buf[],
                  char error_buf[], fd_set &read_sockets, Client clients[],
                  string server_target);
