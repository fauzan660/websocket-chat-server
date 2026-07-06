#include "../headers/room.h"
#include <ctime>
#include <iostream>
#include <string>
using namespace std;

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
void Room::save_room(sqlite3 *db) {
  string sql = "INSERT INTO Rooms (title, creator_id, status) VALUES ('" +
               this->title + "', '" + to_string(this->creator_id) + "', '" +
               this->status + "');";

  char *err;
  if (sqlite3_exec(db, sql.c_str(), NULL, NULL, &err) != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", err);
    sqlite3_free(err);
  }
  this->room_id = sqlite3_last_insert_rowid(db);
  this->content->save_code(db, this->room_id);
}
