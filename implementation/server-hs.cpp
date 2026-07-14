#include "headers/server-hs.h"
#include <arpa/inet.h> // inet_pton
#include <fcntl.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>

using namespace std;

int connect_to_target(string target) {
  string host = target.substr(target.find("//") + 2);
  int port = stoi(host.substr(host.find(":") + 1));
  host = host.substr(0, host.find(":"));

  int fd = socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  inet_pton(AF_INET, host.c_str(), &addr.sin_addr);

  if (connect(fd, (sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("connect to target failed -- is server running?");
    return -1;
  }

  string request = "GET / HTTP/1.1\r\n"
                   "Host: " +
                   host +
                   "\r\n"
                   "Upgrade: websocket\r\n"
                   "Connection: Upgrade\r\n"
                   "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
                   "Sec-WebSocket-Version: 13\r\n\r\n";
  send(fd, request.c_str(), request.size(), 0);

  // drain 101 — blocking, fcntl not set yet
  char buf[1024];
  recv(fd, buf, sizeof(buf), 0);

  if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) {
    perror("fcntl server connection");
    return -1;
  }

  return fd;
}
