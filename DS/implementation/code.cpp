#include "../headers/code.h"
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
void Code::save_code(sqlite3 *db, int room_id) {
  string sql = "INSERT INTO Code (room_id, code_snippet, error_message, "
               "language, context) VALUES ('" +
               to_string(room_id) + "', '" + this->code_snippet + "', '" +
               this->error_message + "', '" + this->language + "', '" +
               this->context + "');";
  char *err;
  if (sqlite3_exec(db, sql.c_str(), NULL, NULL, &err) != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", err);
    sqlite3_free(err);
  }
}
