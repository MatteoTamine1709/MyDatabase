#include "utils.h"

#include <iomanip>
#include <iostream>

#include "string.h"
namespace utils {
void *parseType(const std::string &value, Type type) {
    if (value == "NULL") return nullptr;
    switch (type) {
        case Type::INTEGER: {
            void *integer = malloc(type_min_size[Type::INTEGER]);
            int i = std::stoi(value);
            memcpy(integer, &i, type_min_size[Type::INTEGER]);
            return integer;
        }
        case Type::FLOAT: {
            void *float_ = malloc(type_min_size[Type::FLOAT]);
            float f = std::stof(value);
            memcpy(float_, &f, type_min_size[Type::FLOAT]);
            return float_;
        }
        case Type::DECIMAL: {
            void *decimal = malloc(type_min_size[Type::DECIMAL]);
            double d = std::stod(value);
            memcpy(decimal, &d, type_min_size[Type::DECIMAL]);
            return decimal;
        }
        case Type::CHAR:
        case Type::VARCHAR:
        case Type::TEXT: {
            void *string = malloc(value.size());
            memcpy(string, value.data(), value.size());
            return string;
        }
        case Type::DATE:
            return parseDate(value);
        case Type::TIME:
            return parseTime(value);
        case Type::TIMESTAMP:
            return parseTimestamp(value);
        case Type::BOOLEAN: {
            void *boolean = malloc(type_min_size[Type::BOOLEAN]);
            bool b = value == "true" ? true : false;
            memcpy(boolean, &b, type_min_size[Type::BOOLEAN]);
            return boolean;
        }
        case Type::BIT:
            return parseBit(value);
        case Type::BLOB:
        case Type::VARBINARY:
            return parseBinary(value);
    }
    return nullptr;
}

void *parseDate(const std::string &value) {
    if (value.size() != 10 || value[4] != '-' || value[7] != '-')
        return nullptr;
    void *date = malloc(type_min_size[Type::DATE]);
    std::string year = value.substr(0, 4);
    std::string month = value.substr(5, 2);
    std::string day = value.substr(8, 2);
    // Count day since 1900-01-01
    int days = (std::stoi(year) - 1900) * 365 + (std::stoi(month) - 1) * 30 +
               std::stoi(day);
    memcpy(date, &days, 3);
    return date;
}

void *parseTime(const std::string &value) {
    if (value.size() != 8 || value[2] != ':' || value[5] != ':') return nullptr;
    void *time = malloc(type_min_size[Type::TIME]);
    std::string hour = value.substr(0, 2);
    std::string minute = value.substr(3, 2);
    std::string second = value.substr(6, 2);
    // Count second since 00:00:00
    long seconds =
        std::stoul(hour) * 3600 + std::stoul(minute) * 60 + std::stoul(second);
    memcpy(time, &seconds, 5);
    return time;
}

void *parseTimestamp(const std::string &value) {
    if (value.size() != 19 || value[4] != '-' || value[7] != '-' ||
        value[10] != ' ' || value[13] != ':' || value[16] != ':')
        return nullptr;
    void *timestamp = malloc(type_min_size[Type::TIMESTAMP]);
    std::string year = value.substr(0, 4);
    std::string month = value.substr(5, 2);
    std::string day = value.substr(8, 2);
    std::string hour = value.substr(11, 2);
    std::string minute = value.substr(14, 2);
    std::string second = value.substr(17, 2);
    // Count second since 1900-01-01 00:00:00
    uint64_t seconds = (std::stoul(year) - 1900) * 365 * 24 * 3600 +
                       (std::stoul(month) - 1) * 30 * 24 * 3600 +
                       std::stoul(day) * 24 * 3600 + std::stoul(hour) * 3600 +
                       std::stoul(minute) * 60 + std::stoul(second);
    memcpy(timestamp, &seconds, 8);
    return timestamp;
}

void *parseBit(const std::string &value) {
    std::string bits = value;
    if (bits.size() % 8) bits = std::string(8 - bits.size() % 8, '0') + bits;
    uint64_t size = bits.size() / 8;
    void *bit = malloc(size);
    memset(bit, 0, size);
    for (int i = 0; i < bits.size(); ++i)
        if (bits[i] == '1') ((char *)bit)[i / 8] |= (1 << (i % 8));
    return bit;
}

void *parseBinary(const std::string &value) {
    void *binary = malloc(value.size());
    memcpy(binary, value.data(), value.size());
    return binary;
}

bool isCorrectType(const std::string &value, Type type) {
    switch (type) {
        case Type::INTEGER:
            try {
                std::stoi(value);
            } catch (const std::exception &e) {
                return false;
            }
            return true;
        case Type::FLOAT:
            try {
                std::stof(value);
            } catch (const std::exception &e) {
                return false;
            }
            return true;
        case Type::DECIMAL:
            try {
                std::stod(value);
            } catch (const std::exception &e) {
                return false;
            }
            return true;
        case Type::CHAR:
            return value.size() <= type_max_size[Type::CHAR];
        case Type::VARCHAR:
            return value.size() <= type_max_size[Type::VARCHAR];
        case Type::TEXT:
            return value.size() <= type_max_size[Type::TEXT];
        case Type::DATE:
            return isCorrectDate(value);
        case Type::TIME:
            return isCorrectTime(value);
        case Type::TIMESTAMP:
            return isCorrectTimestamp(value);
        case Type::BOOLEAN:
            return value == "true" || value == "false";
        case Type::BIT:
            return value.size() <= type_max_size[Type::BIT];
        case Type::BLOB:
            return value.size() <= type_max_size[Type::BLOB];
        case Type::VARBINARY:
            return value.size() <= type_max_size[Type::VARBINARY];
        default:
            return false;
    }
}

bool isCorrectDate(const std::string &value) {
    if (value.size() != 10) return false;
    if (value[4] != '-' || value[7] != '-') return false;
    std::string year = value.substr(0, 4);
    std::string month = value.substr(5, 2);
    std::string day = value.substr(8, 2);
    if (year.size() != 4 || month.size() != 2 || day.size() != 2) return false;
    try {
        std::stoi(year);
        std::stoi(month);
        std::stoi(day);
    } catch (const std::exception &e) {
        return false;
    }
    return true;
}

bool isCorrectTime(const std::string &value) {
    if (value.size() != 8) return false;
    if (value[2] != ':' || value[5] != ':') return false;
    std::string hour = value.substr(0, 2);
    std::string minute = value.substr(3, 2);
    std::string second = value.substr(6, 2);
    if (hour.size() != 2 || minute.size() != 2 || second.size() != 2)
        return false;
    try {
        std::stoi(hour);
        std::stoi(minute);
        std::stoi(second);
    } catch (const std::exception &e) {
        return false;
    }
    return true;
}

bool isCorrectTimestamp(const std::string &value) {
    if (value.size() != 19) return false;
    if (value[4] != '-' || value[7] != '-' || value[10] != ' ' ||
        value[13] != ':' || value[16] != ':')
        return false;
    std::string year = value.substr(0, 4);
    std::string month = value.substr(5, 2);
    std::string day = value.substr(8, 2);
    std::string hour = value.substr(11, 2);
    std::string minute = value.substr(14, 2);
    std::string second = value.substr(17, 2);
    if (year.size() != 4 || month.size() != 2 || day.size() != 2 ||
        hour.size() != 2 || minute.size() != 2 || second.size() != 2)
        return false;
    try {
        std::stoi(year);
        std::stoi(month);
        std::stoi(day);
        std::stoi(hour);
        std::stoi(minute);
        std::stoi(second);
    } catch (const std::exception &e) {
        return false;
    }
    return true;
}

uint64_t getTypeSize(const std::string &value, Type type) {
    if (value == "NULL") return 0;
    switch (type) {
        case Type::INTEGER:
            return type_min_size[Type::INTEGER];
        case Type::FLOAT:
            return type_min_size[Type::FLOAT];
        case Type::DECIMAL:
            return type_min_size[Type::DECIMAL];
        case Type::CHAR:
            return value.size();
        case Type::VARCHAR:
            return value.size();
        case Type::TEXT:
            return value.size();
        case Type::DATE:
            return type_min_size[Type::DATE];
        case Type::TIME:
            return type_min_size[Type::TIME];
        case Type::TIMESTAMP:
            return type_min_size[Type::TIMESTAMP];
        case Type::BOOLEAN:
            return type_min_size[Type::BOOLEAN];
        case Type::BIT:
            return ((value.size() - 1) / 8) + 1;
        case Type::BLOB:
            return value.size();
        case Type::VARBINARY:
            return value.size();
        default:
            return 0;
    }
    return 0;
}

std::string getValue(char type, void *value, uint64_t size) {
    if (value == nullptr || size == 0) return "NULL";
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
            memcpy(&days, value, 3);
            uint64_t year = days / 365 + 1900;
            uint64_t month = (days % 365) / 30 + 1;
            uint64_t day = (days % 365) % 30;
            return std::to_string(year) + "-" + std::to_string(month) + "-" +
                   std::to_string(day);
        }
        case Type::TIME: {
            uint64_t seconds = 0;
            // Read 5bytes
            memcpy(&seconds, value, 5);
            uint64_t hour = seconds / 3600;
            uint64_t minute = (seconds % 3600) / 60;
            uint64_t second = (seconds % 3600) % 60;
            return std::to_string(hour) + ":" + std::to_string(minute) + ":" +
                   std::to_string(second);
        }
        case Type::TIMESTAMP: {
            uint64_t seconds = 0;
            // Read 8bytes
            memcpy(&seconds, value, 8);
            uint64_t year = seconds / (365 * 24 * 3600) + 1900;
            uint64_t month =
                (seconds % (365 * 24 * 3600)) / (30 * 24 * 3600) + 1;
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
                (((((seconds % (365 * 24 * 3600)) % (30 * 24 * 3600)) %
                   (24 * 3600)) %
                  3600) %
                 60) %
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

void displaySelection(void *blob) {
    size_t numberOfRows = *(size_t *)blob;
    blob = (char *)blob + sizeof(size_t);
    size_t numberOfColumns = *(size_t *)blob;
    blob = (char *)blob + sizeof(size_t);
    std::cout << "Number of rows: " << numberOfRows << std::endl;
    std::cout << "Number of columns: " << numberOfColumns << std::endl;
    std::vector<std::string> columnNames;
    for (int i = 0; i < numberOfColumns; ++i) {
        columnNames.push_back(std::string((char *)blob));
        blob = (char *)blob + columnNames.back().size() + 1;
    }
    std::vector<char> types;
    for (int i = 0; i < numberOfColumns; ++i) {
        types.push_back(*(char *)blob);
        blob = (char *)blob + sizeof(char);
    }

    size_t offset = 0;
    std::vector<uint64_t> maxWidths(numberOfColumns);
    for (int i = 0; i < numberOfColumns; ++i)
        maxWidths[i] = columnNames[i].size();
    std::vector<std::vector<std::string>> values;
    for (int i = 0; i < numberOfRows; ++i) {
        std::vector<uint64_t> sizes;

        for (int j = 0; j < numberOfColumns; ++j) {
            sizes.push_back(*(uint64_t *)((char *)blob + offset));
            offset += sizeof(uint64_t);
        }
        std::vector<char> is_set;
        for (int j = 0; j < numberOfColumns; ++j) {
            is_set.push_back(*(char *)((char *)blob + offset));
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
                getValue(types[j], (char *)blob + offset, sizes[j]));
            maxWidths[j] = std::max(maxWidths[j], values[i][j].size());
            offset += sizes[j];
        }
    }

    // Table display
    std::cout << "Table display:\n";
    for (int i = 0; i < numberOfColumns; ++i)
        std::cout << std::setfill('-') << std::setw(maxWidths[i] + 4) << "-";
    std::cout << "\n" << std::setfill(' ');
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
        std::cout << "\n";
    }
    for (int i = 0; i < numberOfColumns; ++i)
        std::cout << std::setfill('-') << std::setw(maxWidths[i] + 4) << "-";
    std::cout << "\n" << std::setfill(' ');
}

int reverse_memcmp(const void *s1, const void *s2, size_t n) {
    const unsigned char *p1 = (const unsigned char *)s1 + n - 1;
    const unsigned char *p2 = (const unsigned char *)s2 + n - 1;

    while (n-- > 0) {
        if (*p1 != *p2) {
            return *p1 - *p2;
        }
        p1--;
        p2--;
    }

    return 0;
}
}  // namespace utils