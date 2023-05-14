#include <iomanip>

#include "Database.h"

std::string getValue(char type, void *value, uint64_t size) {
    switch (type) {
        case Type::INTEGER:
            return std::to_string(*(int *)value);
        case Type::FLOAT:
            return std::to_string(*(float *)value);
        case Type::DECIMAL:
            return std::to_string(*(double *)value);
        case Type::CHAR:
        case Type::VARCHAR:
        case Type::TEXT:
        case Type::BLOB:
        case Type::VARBINARY:
            return std::string((char *)value, size);
        case Type::DATE: {
            uint32_t days = 0;
            // Read 3bytes
            for (int i = 0; i < 3; ++i) {
                days <<= 8;
                days |= ((char *)value)[i];
            }
            // Number of days since 1900-01-01
            uint32_t year = days / 365;
            uint32_t month = (days % 365) / 30;
            uint32_t day = (days % 365) % 30;
            return std::to_string(year) + "-" + std::to_string(month) + "-" +
                   std::to_string(day);
        }
        case Type::TIME: {
            uint64_t seconds = 0;
            // Read 5bytes
            for (int i = 0; i < 5; ++i) {
                seconds <<= 8;
                seconds |= ((char *)value)[i];
            }
            uint64_t hour = seconds / 3600;
            uint64_t minute = (seconds % 3600) / 60;
            uint64_t second = (seconds % 3600) % 60;
            return std::to_string(hour) + ":" + std::to_string(minute) + ":" +
                   std::to_string(second);
        }
        case Type::TIMESTAMP: {
            uint64_t seconds = 0;
            // Read 8bytes
            for (int i = 0; i < 8; ++i) {
                seconds <<= 8;
                seconds |= ((char *)value)[i];
            }
            uint64_t year = seconds / (365 * 24 * 3600);
            uint64_t month = (seconds % (365 * 24 * 3600)) / (30 * 24 * 3600);
            uint64_t day = ((seconds % (365 * 24 * 3600)) % (30 * 24 * 3600)) /
                           (24 * 3600);
            uint64_t hour =
                (((seconds % (365 * 24 * 3600)) % (30 * 24 * 3600)) %
                 (24 * 3600)) /
                3600;
            uint64_t minute =
                ((((seconds % (365 * 24 * 3600)) % (30 * 24 * 3600)) %
                  (24 * 3600)) %
                 3600) /
                60;
            uint64_t second =
                ((((seconds % (365 * 24 * 3600)) % (30 * 24 * 3600)) %
                  (24 * 3600)) %
                 3600) %
                60;
            return std::to_string(year) + "-" + std::to_string(month) + "-" +
                   std::to_string(day) + " " + std::to_string(hour) + ":" +
                   std::to_string(minute) + ":" + std::to_string(second);
        }
        case Type::BOOLEAN:
            return (*(bool *)value) == true ? "True" : "False";
        case Type::BIT: {
            std::string bit = "";
            for (int i = 0; i < size; ++i) {
                for (int j = 0; j < 8; ++j) {
                    bit += (((char *)value)[i] & (1 << j)) ? "1" : "0";
                }
            }
            return bit;
        }
        default:
            return "NULL";
    }
}

int main(int argc, char const *argv[]) {
    Database db("User");
    db.createTable("User", {{"id", Type::INTEGER}, {"name", Type::VARCHAR}});
    // db.prettyPrint("User");
    std::cout << db.insert("User", {"id", "name"}, {"1", "user1"}) << std::endl;
    for (int i = 2; i < 100; ++i)
        std::cout << db.insert("User", {"id", "name"},
                               {std::to_string(i), "user" + std::to_string(i)})
                  << std::endl;
    auto [selectionMessage, selection] = db.select("User", {"id", "name"}, {});
    void *selectionPointer = selection;
    std::cout << selectionMessage << std::endl;
    size_t numberOfRows = *(size_t *)selection;
    selection = (char *)selection + sizeof(size_t);
    size_t numberOfColumns = *(size_t *)selection;
    selection = (char *)selection + sizeof(size_t);
    std::cout << "Number of rows: " << numberOfRows << std::endl;
    std::cout << "Number of columns: " << numberOfColumns << std::endl;
    std::vector<char> types;
    for (int i = 0; i < numberOfColumns; ++i) {
        types.push_back(*(char *)selection);
        selection = (char *)selection + sizeof(char);
    }
    for (int i = 0; i < numberOfColumns; ++i)
        std::cout << "Column " << i << ": " << (int)types[i] << std::endl;

    size_t offset = 0;
    std::vector<uint64_t> maxWidths(numberOfColumns, 0);
    std::vector<std::vector<std::string>> values;
    for (int i = 0; i < numberOfRows; ++i) {
        std::vector<uint64_t> sizes;

        for (int j = 0; j < numberOfColumns; ++j) {
            sizes.push_back(*(uint64_t *)((char *)selection + offset));
            offset += sizeof(uint64_t);
        }
        std::vector<char> is_set;
        for (int j = 0; j < numberOfColumns; ++j) {
            is_set.push_back(*(char *)((char *)selection + offset));
            offset += sizeof(char);
        }
        values.push_back(std::vector<std::string>());
        for (int j = 0; j < numberOfColumns; ++j) {
            if (is_set[j] == 0) {
                values[i].push_back("NULL");
                maxWidths[j] = std::max(maxWidths[j], (uint64_t)4);
                offset += sizes[j];
                continue;
            }
            values[i].push_back(
                getValue(types[j], (char *)selection + offset, sizes[j]));
            maxWidths[j] = std::max(maxWidths[j], values[i][j].size());
            offset += sizes[j];
        }
    }

    // Table display
    std::cout << "Table display:\n";
    std::vector<std::string> columnNames = {"id", "name"};
    for (int i = 0; i < numberOfColumns; ++i) {
        std::cout << "| " << std::left << std::setw(maxWidths[i])
                  << columnNames[i] << " |";
    }
    std::cout << std::endl;
    for (int i = 0; i < numberOfColumns; ++i)
        std::cout << std::setfill('-') << std::setw(maxWidths[i] + 4) << "-";
    std::cout << "\n" << std::setfill(' ');
    for (int i = 0; i < numberOfRows; ++i) {
        for (int j = 0; j < numberOfColumns; ++j) {
            std::cout << "| " << std::left << std::setw(maxWidths[j])
                      << values[i][j] << " |";
        }
        std::cout << std::endl;
    }
    for (int i = 0; i < numberOfColumns; ++i)
        std::cout << std::setfill('-') << std::setw(maxWidths[i] + 4) << "-";
    std::cout << "\n" << std::setfill(' ');

    free(selectionPointer);
    return 0;
}
