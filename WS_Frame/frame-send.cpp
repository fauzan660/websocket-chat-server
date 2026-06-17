
#include "frame-send.h"
#include <cstdint>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
using namespace std;

WS_Frame_Client::WS_Frame_Client() {
  this->first_byte = 0x81;
  this->second_byte = 0x00;
}

void WS_Frame_Client::parse_response(char payload[]) {
  this->second_byte = (uint8_t)strlen(payload);

  for (int i = 0; i < strlen(payload); i++) {
    this->payload.push_back((uint8_t)payload[i]);
  }
}

vector<uint8_t> WS_Frame_Client::parse_bytes() {
  this->bytes_response.push_back(this->first_byte);
  this->bytes_response.push_back(this->second_byte);
  this->bytes_response.insert(this->bytes_response.end(), this->payload.begin(),
                              this->payload.end());
  return this->bytes_response;
}
