#include "headers/ws.h"
#include "headers/ws_helpers.h"
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <sys/socket.h>
#include <vector>
using namespace std;

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
  if (con == headers_map.end() ||
      (con->second != "upgrade" && con->second != "Upgrade"))
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
  printf("send was successful\n");

  return send_status;
}
