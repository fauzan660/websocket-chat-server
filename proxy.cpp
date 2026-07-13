#include "proxy.h"
#include "WS_Frame/headers/frame.h"
#include <unistd.h>
ProxyConfig parse_args(int argc, char *argv[]) {
  ProxyConfig cfg;
  for (int i = 1; i < argc; i++) {
    if (string(argv[i]) == "--target")
      cfg.target = argv[++i];
    if (string(argv[i]) == "--port")
      cfg.port = stoi(argv[++i]);
  }
  return cfg;
}
void log_frame(uint8_t *buf, int size, const string &direction) {
  WS_Frame frame;
  frame.parse(buf, size);
  auto payload = frame.get_payload_data();

  string opcode_str;
  switch (frame.get_opcode()) {
  case 0x1:
    opcode_str = "text";
    break;
  case 0x2:
    opcode_str = "binary";
    break;
  case 0x9:
    opcode_str = "ping";
    break;
  case 0xA:
    opcode_str = "pong";
    break;
  default:
    opcode_str = "unknown";
    break;
  }

  printf("[%s] %s | %.*s\n", direction.c_str(), opcode_str.c_str(),
         (int)payload.size(), payload.data());
}
void close_session(ProxySession *sess) {
  close(sess->client_fd);
  close(sess->server_fd);
  sess->client_fd = sess->server_fd = -1;
}
