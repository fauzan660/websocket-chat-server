#pragma once
#include "sqlite3.h"
#include <string>

using namespace std;
class Suggestion {
  int suggestion_id;
  int room_id;
  int author_id;
  string content;
  string created_at;
  bool is_accepted;

public:
  Suggestion(int suggestion_id, int room_id, int author_id);
  void add_suggestion(string content);
  void save_suggestion(sqlite3 *db);
};
