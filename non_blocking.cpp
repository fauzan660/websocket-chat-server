#include "WS_Frame/frame.h"
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
using namespace std;
#define MAX_CONNECTIONS 5
#define MAX_CLIENTS 20
#define BUF_SIZE 4096
#define MAGIC_WEBSOCKET_UUID_STRING "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
#define WS_ENDPOINT "/websocket"
#define MAX_WS_CLIENTS 10
#define WS_BUF_SIZE 20

uint8_t ws_buf[WS_BUF_SIZE];

typedef struct {
  int fd;
} Client;

void close_socket(Client *c) {
  ::close(c->fd);
  c->fd = -1;
}

string encode_key_openssl(string key) {
  unsigned char hash[SHA_DIGEST_LENGTH];
  SHA1((unsigned char *)key.c_str(), key.size(), hash);
  return string(hash, hash + SHA_DIGEST_LENGTH);
}
string encode_key_base64(string &key) {
  string output(((key.size() + 2) / 3) * 4, '\0');
  EVP_EncodeBlock((unsigned char *)output.data(), (unsigned char *)key.c_str(),
                  key.size());
  return output;
}

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

int setup_server_socket(int &s, struct sockaddr_in &address, int PORT) {
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);
  cout << "hello" << endl;
  if ((s = ::socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("timeout while socket creation");
    exit(-1);
  }
  int opt = 1;
  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  if (::bind(s, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("socket with fd not binded correctly to address");
    exit(-1);
  }
  if (fcntl(s, F_SETFL, O_NONBLOCK) < 0) {
    perror("making socket non blocking failed!!");
    exit(-1);
  }
  if (::listen(s, MAX_CONNECTIONS) < 0) {
    perror("socket is listening at port");
    exit(-1);
  }
  return 1;
}

string generate_websocket_response_key(string websocket_request_key) {
  string magic_string_concat =
      websocket_request_key + MAGIC_WEBSOCKET_UUID_STRING;
  string hashed_string = encode_key_openssl(magic_string_concat);
  if (hashed_string.empty())
    return "";
  string encoded_string = encode_key_base64(hashed_string);
  if (encoded_string.empty())
    return "";

  return encoded_string;
}
bool check_request_ws(string method, string target, string http_version,
                      map<string, string> headers_map) {

  if (method != "GET") {
    printf("check ws request: method is not GET \n");
    return false;
  }
  string http_version_number =
      http_version.substr(http_version.find('/') + 1, http_version.length());
  float http_vn = stof(http_version_number);
  if (http_vn < 1.1) {

    printf("check ws request: http version is deprecated \n");
    return false;
  }
  auto upg = headers_map.find("upgrade");
  if (upg == headers_map.end() && upg->second != "websocket") {
    printf("check ws request: upgrade header was missing or incorrect \n");
    return false;
  }
  auto con = headers_map.find("connection");
  if (con == headers_map.end() && con->second != "Upgrade") {
    printf("check ws request: connection header was missing or incorrect \n");
    return false;
  }
  auto key = headers_map.find("sec-websocket-key");
  if (key == headers_map.end()) {
    printf("check ws request: ws key not present in headers \n");
    return false;
  }
  printf("All keys present in WS request switching protocols \n");
  return true;
}
int handle_request_ws(Client *c, map<string, string> headers_map,
                      Client websocket_clients[], Client clients[]) {

  string ws_response = "HTTP/1.1 101 Switching Protocols\r\n"
                       "Upgrade: websocket\r\n"
                       "Connection: Upgrade\r\n";
  auto key = headers_map.find("sec-websocket-key");
  if (key != headers_map.end()) {
    string ws_response_key = generate_websocket_response_key(key->second);
    if (ws_response_key.empty()) {
      printf("error generating ws accept key \n");
      return -1;
    }
    ws_response += "Sec-WebSocket-Accept: " + ws_response_key + "\r\n\r\n";
    printf("server generate ws response: \n%s \n", ws_response.c_str());
    int send_status =
        ::send(c->fd, ws_response.c_str(), strlen(ws_response.c_str()), 0);
    printf("handshake response length: %zu\n", strlen(ws_response.c_str()));

    printf("client %d handshake send status: %d\n", c->fd, send_status);
    if (send_status < 0) {
      perror("send");
    }
    for (int i = 0; i < MAX_WS_CLIENTS; i++) {
      if (websocket_clients[i].fd == -1) {
        websocket_clients[i].fd = c->fd;
        for (int j = 0; j < MAX_CLIENTS; j++) {
          if (clients[j].fd == c->fd) {
            clients[j].fd = -1;
            break;
          }
        }
        break;
      }
    }
    return send_status;
  } else {
    perror("ws key not found in headers \n");
  }
  return -1;
}

void accept_clients(int s, Client clients[], fd_set &read_sockets,
                    struct sockaddr_in &address, int &address_len) {
  cout << "-------Accepting client connections------" << endl;
  while (true) {
    int client =
        accept(s, (struct sockaddr *)&address, (socklen_t *)&address_len);
    if (client < 0) {
      printf("no more client left exiting.....\n");
      break;
    }
    if (fcntl(client, F_SETFL, O_NONBLOCK) < 0)
      perror("Error making client socket non blocking");
    FD_SET(client, &read_sockets);
    cout << "client " << client << " admitted to readable sockets set" << endl;
    for (int i = 0; i < MAX_CLIENTS; i++) {
      if (clients[i].fd == -1) {
        clients[i].fd = client;
        break;
      }
    }
  }
}

void handle_client(Client *c, char server_buf[], char response_buf[],
                   char error_buf[], fd_set &read_sockets,
                   Client websocket_clients[], Client clients[]) {

  string method, target, http_version;
  map<string, string> headers_map;
  cout << "-------Handling client connections-------" << endl;
  string message = "";
  while (true) {
    int recv_status = ::recv(c->fd, server_buf, BUF_SIZE - 1, 0);
    if (recv_status == 0) {
      perror("recv failed -- timeout --");
      close_socket(c);
      break;
    }
    if (recv_status < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK)
        break;
      perror("recv failed -- connection closed --");
      close_socket(c);
      break;
    }
    server_buf[recv_status] = '\0';
    message += string(server_buf, recv_status);
    if (message.size() > 4 && message.substr(message.size() - 4) == "\r\n\r\n")
      break;
  }
  if (c->fd == -1 || message.empty())
    return;
  cout << "Received message:\n" << message << endl;
  parse_request(message, method, target, http_version, headers_map);
  cout << "method, target, http_version: " << method << " " << target << " "
       << http_version << endl;

  cout << "target --> " << target << endl;

  int client_fd = c->fd;
  if (target == "/websocket") {
    printf("request to ws endpoint \n");
    if (check_request_ws(method, target, http_version, headers_map)) {
      int handle_request_ws_status =
          handle_request_ws(c, headers_map, websocket_clients, clients);
      if (handle_request_ws_status) {
        printf("client %d request handled by server successfully \n",
               client_fd);
      } else {
        perror("request handling failed");
        ::send(c->fd, error_buf, strlen(error_buf), 0);
      }
    } else {
      printf("not a valid ws request \n");
      ::send(c->fd, error_buf, strlen(error_buf), 0);
      close(c->fd);
    }
  } else {
    printf("request to http endpoint \n");
    ::send(c->fd, response_buf, strlen(response_buf), 0);
  }
}

void handle_websocket_client(Client *ws_c, Client clients[],
                             Client websocket_clients[]) {
  printf("Handling connections with websocket \n");
  int recv_status = ::recv(ws_c->fd, ws_buf, BUF_SIZE - 1, 0);
  if (recv_status == 0) {
    perror("recv failed -- timeout --");
    close_socket(ws_c);
    return;
  }
  if (recv_status < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK)
      return;
    perror("recv failed -- connection closed --");
    close_socket(ws_c);
    return;
  }
  WS_Frame frame = WS_Frame();
  frame.parse(ws_buf, sizeof(ws_buf));
  printf("%.*s\n", (int)frame.get_payload_data().size(),
         frame.get_payload_data().data());
}

int main() {
  int s;
  const int PORT = 8081;
  struct sockaddr_in address;
  fd_set write_sockets, read_sockets;
  struct timeval waitd;
  Client clients[MAX_CLIENTS];
  Client websocket_clients[MAX_WS_CLIENTS];
  char server_buf[BUF_SIZE];
  char response_buf[] = "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/html; charset=utf-8\r\n"
                        "\r\n"
                        "<HTML><HEAD>"
                        "<meta http-equiv=\"content-type\" "
                        "content=\"text/html;charset=utf-8\">\r\n"
                        "<TITLE>200 OK</TITLE></HEAD><BODY>\r\n"
                        "<H1>200 OK</H1>\r\n"
                        "Welcome to the default.\r\n"
                        "</BODY></HTML>\r\n";
  char ws_handshake_error_buf[] = "HTTP/1.1 400 Bad Request";

  if (setup_server_socket(s, address, PORT))
    printf("Server socket successfully setup\n");

  int max_fd = s;
  int address_len = sizeof(address);
  cout << "listening on port " << PORT << endl;
  FD_ZERO(&write_sockets);
  FD_ZERO(&read_sockets);
  FD_SET(s, &read_sockets);
  waitd.tv_sec = 10;

  for (int i = 0; i < MAX_CLIENTS; i++)
    clients[i].fd = -1;
  for (int i = 0; i < MAX_WS_CLIENTS; i++)
    websocket_clients[i].fd = -1;

  while (1) {
    max_fd = s;
    FD_ZERO(&read_sockets);
    FD_SET(s, &read_sockets);
    for (int i = 0; i < MAX_CLIENTS; i++) {
      Client *c = &clients[i];
      if (c->fd != -1) {
        FD_SET(c->fd, &read_sockets);
        max_fd = max(max_fd, c->fd);
      }
    }
    for (int i = 0; i < MAX_WS_CLIENTS; i++) {
      Client *ws_c = &websocket_clients[i];
      if (ws_c->fd != -1) {
        FD_SET(ws_c->fd, &read_sockets);
        max_fd = max(max_fd, ws_c->fd);
      }
    }
    int sel = ::select(max_fd + 1, &read_sockets, &write_sockets, (fd_set *)0,
                       &waitd);
    if (sel < 0)
      continue;

    for (int i = 0; i < MAX_WS_CLIENTS; i++) {
      Client *ws_c = &websocket_clients[i];
      if (ws_c->fd != -1 && FD_ISSET(ws_c->fd, &read_sockets)) {
        handle_websocket_client(ws_c, clients, websocket_clients);
        ws_c->fd = -1;
      }
    }

    if (FD_ISSET(s, &read_sockets))
      accept_clients(s, clients, read_sockets, address, address_len);

    for (int i = 0; i < MAX_CLIENTS; i++) {
      Client *c = &clients[i];
      if (c->fd == -1 || c->fd == s)
        continue;
      if (FD_ISSET(c->fd, &read_sockets))
        handle_client(c, server_buf, response_buf, ws_handshake_error_buf,
                      read_sockets, websocket_clients, clients);
    }
  }
  return 0;
}
