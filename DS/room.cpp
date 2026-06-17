#include "room.h"
#include <ctime>
#include <iostream>
using namespace std;

Code::Code() {
  this->code_snippet = "";
  this->error_message = "";
  this->context = "";
  this->language = "";
}
void Code::add(string code_snippet, string error_message, string language,
               string context) {
  this->code_snippet = code_snippet;
  this->error_message = error_message;
  this->language = language;
  this->context = context;
}

Room::Room(int room_id, string title, int creator_id) {
  this->room_id = room_id;
  this->title = title;
  this->status = "open";
  time_t now = time(nullptr);
  this->created_at = ctime(&now);
  this->creator_id = creator_id;
  this->content = nullptr;
}

void Room::add_content(string code_snippet, string error_message,
                       string language, string context) {
  this->content = new Code();
  this->content->add(code_snippet, error_message, language, context);
}

Suggestion::Suggestion(int suggestion_id, int room_id, int author_id) {
  this->suggestion_id = suggestion_id;
  this->room_id = room_id;
  this->author_id = author_id;
  this->content = "";
  time_t now = time(nullptr);
  this->created_at = ctime(&now);
  this->is_accepted = false;
}
void Suggestion::add_suggestion(string content) { this->content = content; }
