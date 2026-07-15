#pragma once
#include <iostream>

using namespace std;

string encode_key_openssl(string key);
string encode_key_base64(string &key);
string generate_websocket_response_key(string websocket_request_key);
