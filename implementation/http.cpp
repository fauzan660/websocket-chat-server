#include "../headers/http.h"
#include "../headers/ws.h"
#include "headers/server-hs.h"
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <errno.h>
#include <iostream>
#include <string>
#include <sys/socket.h>
using namespace std;

void parse_request(string message, string &method, string &target,
                   string &http_version, map<string, string> &headers_map) {
  string head = message.substr(0, message.find("\r\n\r\n"));
  string first_line = head.substr(0, head.find("\r\n"));
  size_t s1 = first_line.find(' ');
  size_t s2 = first_line.find(' ', s1 + 1);
  method = first_line.substr(0, s1);
  target = first_line.substr(s1 + 1, s2 - s1 - 1);
  http_version = first_line.substr(s2 + 1);
  string headers_str = head.substr(head.find("\r\n") + 2);
  size_t pos = 0;
  while ((pos = headers_str.find("\r\n")) != string::npos) {
    string line = headers_str.substr(0, pos);
    size_t colon = line.find(": ");
    if (colon != string::npos) {
      string key = line.substr(0, colon);
      string val = line.substr(colon + 2);
      for (auto &ch : key)
        ch = tolower(ch);
      headers_map[key] = val;
    }
    headers_str = headers_str.substr(pos + 2);
  }
  if (!headers_str.empty()) {
    size_t colon = headers_str.find(": ");
    if (colon != string::npos) {
      string key = headers_str.substr(0, colon);
      string val = headers_str.substr(colon + 2);
      for (auto &ch : key)
        ch = tolower(ch);
      headers_map[key] = val;
    }
  }
}
string read_http_message(Client *c, char server_buf[]) {
  string message = "";
  while (true) {
    int recv_status = recv(c->fd, server_buf, BUF_SIZE - 1, 0);
    printf("recv_status: %d errno: %d\n", recv_status, errno);
    if (recv_status == 0) {
      close_socket(c);
      return "";
    }
    if (recv_status < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK)
        return message;
      close_socket(c);
      return "";
    }
    server_buf[recv_status] = '\0';
    message += string(server_buf, recv_status);
    if (message.size() > 4 && message.substr(message.size() - 4) == "\r\n\r\n")
      break;
  }
  return message;
}

int handle_client(Client *c, char server_buf[], char response_buf[],
                  char error_buf[], fd_set &read_sockets, Client clients[],
                  string server_target) {
  char body[] = "<title>WS Endpoint</title>"
                "<h2>you have hit the ws endpoint with an http request</h2>";
  char http_buffer[1024];
  snprintf(http_buffer, sizeof(http_buffer),
           "HTTP/1.1 200 OK\r\nContent-Type: text/html; "
           "charset=utf-8\r\nContent-Length: %zu\r\n\r\n%s",
           strlen(body), body);
  string method, target, http_version;
  map<string, string> headers_map;

  cout << "-------Handling client connections-------" << endl;
  string message = read_http_message(c, server_buf);
  if (c->fd == -1 || message.empty()) {
    printf("HTTP meesage is empty: not ready yet\n");
    return -1;
  }
  cout << "Received message:\n" << message << endl;
  parse_request(message, method, target, http_version, headers_map);

  if (check_request_ws(method, target, http_version, headers_map)) {
    int status = handle_request_ws(c, headers_map, clients);
    if (status <= 0) {
      perror("request handling failed");
      ::send(c->fd, error_buf, strlen(error_buf), 0);
    } else {
      int server_fd = connect_to_target(server_target);
      return server_fd;
    }
  } else {
    printf("request at ws endpoint is not a websocket request \n");
    ::send(c->fd, http_buffer, strlen(http_buffer), 0);
    close(c->fd);
  }
  return -1;
}
