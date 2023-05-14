#include "Database.h"

int main(int argc, char const *argv[]) {
    Database db("User");
    db.createTable("User", {{"id", Type::INTEGER}, {"name", Type::VARCHAR}});
    db.insert("User", {"id", "name"}, {"1", "user1"});
    db.insert("User", {"id", "name"}, {"2", "user2"});
    return 0;
}
