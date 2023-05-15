#include <chrono>
#include <iomanip>

#include "Database.h"
#include "utils.h"

int main(int argc, char const *argv[]) {
    Database db("User");
    db.createTable("User", {{"id", Type::INTEGER},
                            {"name", Type::VARCHAR},
                            {"createdAt", Type::TIMESTAMP},
                            {"updatedAt", Type::TIMESTAMP},
                            {"dayOfBirth", Type::DATE},
                            {"timeOfBirth", Type::TIME},
                            {"isMale", Type::BOOLEAN},
                            {"height", Type::FLOAT},
                            {"weight", Type::DECIMAL},
                            {"avatar", Type::BLOB},
                            {"password", Type::VARBINARY},
                            {"role", Type::BIT}});
    // Start chrono
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 1; i < 1000000; ++i)
        db.insert("User", {"id", "name", "role"},
                  {std::to_string(i), "user" + std::to_string(i), "10000000"});
    // Stop chrono
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Insert time: " << duration.count() << " microseconds"
              << std::endl;

    // Start chrone
    start = std::chrono::high_resolution_clock::now();
    // for (int i = 1; i < 1000; ++i) {
    auto [selectionMessage, selection] =
        db.select("User", {"id", "name", "role"}, {});
    // utils::displaySelection(selection);
    free(selection);
    // }
    // Stop chrono
    stop = std::chrono::high_resolution_clock::now();
    duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Select time: " << duration.count() << " microseconds"
              << std::endl;
    // }
    // db.prettyPrint("User");
    return 0;
}
