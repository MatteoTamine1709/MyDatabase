#include <iomanip>

#include "Database.h"
#include "utils.h"

int main(int argc, char const *argv[]) {
    Database db("User");
    db.createTable("User", {
                               {"id", Type::INTEGER},
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
                           });
    for (int i = 1; i < 2; ++i)
        std::cout << db.insert("User",
                               {"id", "name", "createdAt", "dayOfBirth",
                                "timeOfBirth", "height", "weight", "isMale"},
                               {std::to_string(i), "user" + std::to_string(i),
                                "2001-09-17 01:00:00", "2001-09-17", "01:00:00",
                                "0.4", "12.9", rand() % 2 ? "true" : "false"})
                  << std::endl;

    auto [selectionMessage, selection] = db.select(
        "User",
        {"dayOfBirth", "createdAt", "id", "height", "weight", "isMale", "name"},
        {});
    std::cout << selectionMessage << std::endl;
    utils::displaySelection(selection);
    free(selection);
    return 0;
}
