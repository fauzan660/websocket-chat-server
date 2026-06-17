#pragma once
#include <cstdint>
#include <ctime>
#include <iostream>
#include <sqlite3.h>
#include <vector>
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
};

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
};

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
};
