#pragma once
#include <cstdint>
#include <iostream>
#include <vector>
using namespace std;
#include <string>

class WS_Frame {
  uint8_t *data_in_bytes;
  uint8_t first_byte;
  uint8_t fin;
  uint8_t rsv1;
  uint8_t rsv2;
  uint8_t rsv3;
  uint8_t opcode;
  uint8_t second_byte;
  uint8_t mask;           /* flag  */
  uint8_t mask_key_start; /* index */
  uint8_t masking_key[4];
  uint64_t payload_length;
  vector<uint8_t> payload_data;

  void parse_flags(uint8_t *data);
  void parse_payload_length(uint8_t *data);
  void maybe_parse_masking_key(uint8_t *data);
  void parse_payload(uint8_t *data, int data_buf_size);

public:
  WS_Frame();
  void parse(uint8_t *data, int data_buf_size);
  vector<uint8_t> get_payload_data();
};
