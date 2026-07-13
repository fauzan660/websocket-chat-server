#pragma once
#include <string>
using namespace std;

struct ProxyConfig {
  string target;
  int port;
};
struct ProxySession {
  int client_fd; // client A
  int server_fd; // client A's dedicated server connection
};
ProxyConfig parse_args(int argc, char *argv[]);

void log_frame(uint8_t *buf, int size, const string &direction);

void close_session(ProxySession *sess);
