#include <chrono>
#include <iomanip>

#include "Database.h"
#include "utils.h"

int main(int argc, char const *argv[]) {
    Database db("Database");
    db.load("User");
    db.prettyPrint("User");
    auto start = std::chrono::high_resolution_clock::now();
    // // for (int i = 1; i < 100; ++i) {
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
    db.prettyPrint("User");
    db.dropTable("User");
    // db.createTable("User", {{"id", Type::INTEGER},
    //                         {"name", Type::VARCHAR},
    //                         {"createdAt", Type::TIMESTAMP},
    //                         {"updatedAt", Type::TIMESTAMP},
    //                         {"dayOfBirth", Type::DATE},
    //                         {"timeOfBirth", Type::TIME},
    //                         {"isMale", Type::BOOLEAN},
    //                         {"role", Type::BIT}});
    // // // // Start chrono
    // // auto start = std::chrono::high_resolution_clock::now();
    // for (int i = 100; i < 150; ++i)
    //     db.insert("User", {"id", "name", "role"},
    //               {std::to_string(i), "user" + std::to_string(i),
    //               "10000000"});
    // auto start = std::chrono::high_resolution_clock::now();
    // // // for (int i = 1; i < 100; ++i) {
    // auto [selectionMessage, selection] =
    //     db.select("User", {"id", "name", "role"}, {});
    // utils::displaySelection(selection);
    // free(selection);
    // // // }
    // // Stop chrono
    // auto stop = std::chrono::high_resolution_clock::now();
    // auto duration =
    //     std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    // std::cout << "Select time: " << duration.count() << " microseconds"
    //           << std::endl;
    // db.save();
    // db.prettyPrint("User");
    return 0;
}
