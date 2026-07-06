#include "../headers/action_handler.h"
#include "../headers/db.h"
#include "DS/headers/code.h"
#include "DS/headers/room.h"
#include "DS/headers/suggestion.h"
#include "DS/headers/user.h"
#include "headers/db.h"
#include "sqlite3.h"
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <sys/socket.h>
using namespace std;
using json = nlohmann::json;

void create_user(json data) {
  sqlite3 *db = init_db();

  json payload = data["payload"];
  int user_id = payload["user_id"];
  string username = payload["username"];

  User *user = new User();
  user->create_user(user_id, username);
  user->save_user(db);

  string query = "SELECT * FROM Users;";

  sqlite3_exec(db, query.c_str(), callback, NULL, NULL);
}

void create_room(json data) {
  sqlite3 *db = init_db();
  json payload = data["payload"];
  int room_id = payload["room_id"];
  string title = payload["title"];
  int creator_id = payload["creator_id"];
  json code = payload["code"];
  string code_snippet = code["code_snippet"];
  string error_message = code["error_message"];
  string language = code["language"];
  string context = code["context"];

  Room *room = new Room(room_id, title, creator_id);
  room->add_content(code_snippet, error_message, language, context);

  room->save_room(db);
  string query = "SELECT * FROM Rooms r JOIN Code c ON r.room_id = c.room_id;";
  char *err;
  if (sqlite3_exec(db, query.c_str(), callback, NULL, &err) != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", err);
    sqlite3_free(err);
  }
}

void view_room_suggestions(json data) {
  sqlite3 *db = init_db();
  json payload = data["payload"];
  int room_id = payload["room_id"];
  string room_id_str = to_string(room_id);

  string query = "SELECT * FROM Rooms r JOIN Suggestions s  ON r.room_id = "
                 "s.room_id WHERE r.room_id = " +
                 room_id_str + ";";
  char *err;
  if (sqlite3_exec(db, query.c_str(), callback, NULL, &err) != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", err);
    sqlite3_free(err);
  }
}
void add_suggestion(json data) {
  sqlite3 *db = init_db();
  json payload = data["payload"];
  int suggestion_id = payload["suggestion_id"];
  int room_id = payload["room_id"];
  int author_id = payload["author_id"];
  Suggestion *suggestion = new Suggestion(suggestion_id, room_id, author_id);
  string content = payload["content"];
  suggestion->add_suggestion(content);
  suggestion->save_suggestion(db);
  string query =
      "SELECT * FROM Rooms r JOIN Suggestions s  ON r.room_id = s.room_id;";
  char *err;
  if (sqlite3_exec(db, query.c_str(), callback, NULL, &err) != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", err);
    sqlite3_free(err);
  }
}
void accept_suggestion(json data) {
  sqlite3 *db = init_db();

  json payload = data["payload"];
  int suggestion_id = payload["suggestion_id"];
  int room_id = payload["room_id"];

  string query =
      "UPDATE Suggestions "
      "SET "
      "is_accepted = 1 "
      "WHERE "
      "suggestion_id = " +
      to_string(suggestion_id) +
      ";"
      "UPDATE Rooms "
      "SET "
      "status = 'closed' "
      "WHERE "
      "room_id = " +
      to_string(room_id) +
      ";"
      "SELECT * FROM Rooms r JOIN Suggestions s  ON r.room_id = s.room_id;";
  ;
  char *err;
  if (sqlite3_exec(db, query.c_str(), callback, NULL, &err) != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", err);
    sqlite3_free(err);
  }
}

void handle_action(Action action, json json_request) {
  switch (action) {
  case Action::CREATE_USER:
    create_user(json_request);
    break;
  case Action::CREATE_ROOM:
    create_room(json_request);
    break;
  case Action::VIEW_ROOM_SUGGESTIONS:
    view_room_suggestions(json_request);
    break;
  case Action::ADD_SUGGESTION:
    add_suggestion(json_request);
    break;
  case Action::ACCEPT_SUGGESTION:
    accept_suggestion(json_request);
    break;
  case Action::UNKNOWN:
    cout << "[ERROR]The action entered is unknown" << endl;
    break;
  }
}
