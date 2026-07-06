#pragma once
#include "code.h"
#include <cstdint>
#include <ctime>
#include <iostream>
#include <sqlite3.h>
#include <string>
#include <vector>
using namespace std;

class Room {
  int room_id;
  string title;
  string status;
  string created_at;
  int creator_id;
  Code *content;

public:
  Room(int room_id, string title, int creator_id);
  void add_content(string code_snippet, string error_message, string language,
                   string context);
  void save_room(sqlite3 *db);
};
