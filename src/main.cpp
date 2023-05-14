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
    for (int i = 1; i < 3; ++i)
        std::cout << db.insert("User", {"id", "name", "role"},
                               {std::to_string(i), "user" + std::to_string(i),
                                "10000000"})
                  << std::endl;

    auto [selectionMessage, selection] =
        db.select("User", {"id", "name", "role"}, {});
    std::cout << selectionMessage << std::endl;
    utils::displaySelection(selection);
    free(selection);
    return 0;
}
