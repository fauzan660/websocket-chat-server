#pragma once
#include "sqlite3.h"
#include <string>
using namespace std;
class Code {
  string code_snippet;
  string error_message;
  string language;
  string context;

public:
  Code();
  void add(string code_snippet, string error_message, string language,
           string context);
  void save_code(sqlite3 *db, int room_id);
};
