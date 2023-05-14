#include "Database.h"

int main(int argc, char const *argv[]) {
    Database db("User");
    db.createTable("User", {{"id", Type::INTEGER}, {"name", Type::VARCHAR}});
    // db.prettyPrint("User");
    std::cout << db.insert("User", {"id", "name"}, {"1", "user1"}) << std::endl;
    for (int i = 2; i < 100; ++i)
        std::cout << db.insert("User", {"id", "name"},
                               {std::to_string(i), "user" + std::to_string(i)})
                  << std::endl;
    // db.insert("User", {"id", "name"}, {"2", "user2"});
    return 0;
}
