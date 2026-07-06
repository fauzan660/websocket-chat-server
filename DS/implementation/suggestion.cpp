#include "../headers/suggestion.h"
#include <string>
using namespace std;
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
void Suggestion::save_suggestion(sqlite3 *db) {
  string sql = "INSERT INTO Suggestions (room_id, author_id, content, "
               "created_at, is_accepted) VALUES ('" +
               to_string(this->room_id) + "', '" + to_string(this->author_id) +
               "', '" + this->content + "', '" + this->created_at + "', '" +
               to_string(this->is_accepted) + "');";
  char *err;
  if (sqlite3_exec(db, sql.c_str(), NULL, NULL, &err) != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", err);
    sqlite3_free(err);
  }
}
