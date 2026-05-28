#include "frame.h"
#include <cstdint>
#include <sys/socket.h>
#include <unistd.h>

WS_Frame::WS_Frame() {
  data_in_bytes = nullptr;
  first_byte = 0;
  fin = 0;
  rsv1 = 0;
  rsv2 = 0;
  rsv3 = 0;
  opcode = 0;
  second_byte = 0;
  mask = 0;
  mask_key_start = 0;
  masking_key[0] = masking_key[1] = masking_key[2] = masking_key[3] = 0;
  payload_length = 0;
}

void WS_Frame::parse(uint8_t *data, int data_buf_size) {
  parse_flags(data);
  parse_payload_length(data);
  maybe_parse_masking_key(data);
  parse_payload(data, data_buf_size);
}

void WS_Frame::parse_flags(uint8_t *data) {
  this->first_byte = *data;
  this->fin = first_byte & 0b10000000;
  this->rsv1 = first_byte & 0b01000000;
  this->rsv2 = first_byte & 0b00100000;
  this->rsv3 = first_byte & 0b00010000;
  this->opcode = first_byte & 0b00001111;
  this->second_byte = *(data + 1);
  this->mask = second_byte & 0b10000000;
}
void WS_Frame::parse_payload_length(uint8_t *data) {
  uint8_t initial_length = *(data + 1) & 0b01111111;
  int mask_key_start = 2;
  uint64_t payload_length = 0;
  int bytes_extend;
  if (initial_length == 126) {
    bytes_extend = 2;
    for (int i = 0; i < bytes_extend; i++) {
      payload_length = (payload_length << 8) | *(data + mask_key_start);
      mask_key_start++;
    }
  } else if (initial_length == 127) {
    bytes_extend = 8;
    for (int i = 0; i < bytes_extend; i++) {
      payload_length = (payload_length << 8) | *(data + mask_key_start);
      mask_key_start++;
    }
  } else {
    payload_length = initial_length;
  }
  this->mask_key_start = mask_key_start;
  this->payload_length = payload_length;
}

void WS_Frame::maybe_parse_masking_key(uint8_t *data) {
  if (!this->mask) {
    printf("check ws frame: mask flag is set to 0 \n");
    return;
  }
  for (int i = 0; i < 4; i++) {
    this->masking_key[i] = *(data + this->mask_key_start + i);
  }
}
void WS_Frame::parse_payload(uint8_t *data, int data_buf_size) {
  if (this->payload_length == 0) {
    printf("check ws frame: payload length is 0 \n");
    return;
  }
  if (this->mask == 0) {
    printf("check ws frame: mask flag set to 0");
    for (int i = mask_key_start; i < data_buf_size; i++) {
      this->payload_data.push_back(*(data + i));
    }
  } else {
    int payload_start = this->mask_key_start + 4;
    int payload_size = data_buf_size - payload_start;
    uint8_t encoded_payload[payload_size];
    uint8_t decoded_payload[payload_size];
    for (int i = 0; i < payload_size; i++) {
      encoded_payload[i] = *(data + payload_start + i);
    }
    for (int i = 0; i < payload_size; i++) {
      decoded_payload[i] = encoded_payload[i] ^ this->masking_key[i % 4];
    }
    for (int i = 0; i < payload_size; i++) {
      this->payload_data.push_back(decoded_payload[i]);
    }
  }
}
vector<uint8_t> WS_Frame::get_payload_data() { return this->payload_data; }
