#include <chrono>
#include <iomanip>

#include "Database.h"
#include "utils.h"

void saveLoadWorking() {
    Database db("Database");
    db.load("User");
    db.prettyPrint(Index("User", "id"));
    auto start = std::chrono::high_resolution_clock::now();
    // // // for (int i = 1; i < 100; ++i) {
    auto [selectionMessage, selection] =
        db.select("User", {"id", "name", "role"}, {});
    utils::displaySelection(selection);
    free(selection);
    // // }
    // Stop chrono
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Select time: " << duration.count() << " microseconds"
              << std::endl;
    // db.prettyPrint("User");
    // db.insert("User", {"id", "name", "role"},
    //           {std::to_string(99), "user" + std::to_string(99), "10000000"});
    // db.dropTable("User");
    // db.createTable("User", {{"id", Type::INTEGER},
    //                         {"name", Type::VARCHAR},
    //                         {"createdAt", Type::TIMESTAMP},
    //                         {"updatedAt", Type::TIMESTAMP},
    //                         {"dayOfBirth", Type::DATE},
    //                         {"timeOfBirth", Type::TIME},
    //                         {"isMale", Type::BOOLEAN},
    //                         {"role", Type::BIT}});
    // for (int i = 99; i < 150; ++i)
    // db.insert("User", {"id", "name", "role"},
    //           {std::to_string(98), "user" + std::to_string(98), "10000000"});
    // db.createIndex("User", {"name"});
    db.prettyPrint(Index("User", "id"));
    db.save();
}

void creatingIndexes() {
    Database db("Database");
    db.load("User");
    // db.createTable("User", {
    //                            {"id", Type::INTEGER},
    //                            {"count", Type::INTEGER},
    //                            {"name", Type::VARCHAR},
    //                            {"createdAt", Type::TIMESTAMP},
    //                            {"isActive", Type::BOOLEAN},

    //                        });
    // set seed for random
    // std::srand(std::time(nullptr));
    // for (int i = 0; i < 100; ++i) {
    //     std::cout << db.insert(
    //                      "User",
    //                      {"id", "count", "name", "createdAt", "isActive"},
    //                      {std::to_string(i), std::to_string(std::rand() %
    //                      1000),
    //                       "user" + std::to_string(i),
    //                       "2021-05-20 00:00:0" + std::to_string(i % 10),
    //                       std::rand() % 2 ? "true" : "false"})
    //               << std::endl;
    //     ;
    // }
    db.prettyPrint(Index("User", "id"));
    // db.createIndex("User", {"count"});
    db.prettyPrint(Index("User", "count"));

    // auto [selectionMessage, selection] =
    //     db.select("User", {"id", "count", "name", "createdAt", "isActive"},
    //               {{Type::INTEGER, "50", ">="}, {Type::INTEGER, "60",
    //               "<="}});
    // if (selectionMessage != "OK") {
    //     std::cout << selectionMessage << std::endl;
    //     return;
    // }
    // utils::displaySelection(selection);
    // free(selection);
    // db.save();
}

int main(int argc, char const *argv[]) {
    creatingIndexes();
    return 0;
}
