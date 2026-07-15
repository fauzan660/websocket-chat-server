#include "headers/ws_helpers.h"
#include <openssl/evp.h>
#include <openssl/sha.h>
#define MAGIC_WEBSOCKET_UUID_STRING "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

using namespace std;
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

string generate_websocket_response_key(string websocket_request_key) {
  string concat = websocket_request_key + MAGIC_WEBSOCKET_UUID_STRING;
  string hashed = encode_key_openssl(concat);
  if (hashed.empty())
    return "";
  string encoded = encode_key_base64(hashed);
  if (encoded.empty())
    return "";
  return encoded;
}
