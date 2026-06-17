#pragma once
#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>
using namespace std;

class WS_Frame_Client {
  uint8_t first_byte;
  uint8_t second_byte;
  vector<uint8_t> payload;
  vector<uint8_t> bytes_response;

public:
  WS_Frame_Client();
  void parse_response(char payload[]);
  vector<uint8_t> parse_bytes();
};
