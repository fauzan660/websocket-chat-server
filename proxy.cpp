#include "proxy.h"

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
