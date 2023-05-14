#include "utils.h"

#include "string.h"

namespace utils {
void *parseType(std::string &value, Type type, uint64_t size) {
    switch (type) {
        case Type::INTEGER: {
            void *integer = malloc(type_min_size[Type::INTEGER]);
            memcpy(integer, &value, type_min_size[Type::INTEGER]);
            return integer;
        }
        case Type::FLOAT: {
            void *float_ = malloc(type_min_size[Type::FLOAT]);
            memcpy(float_, &value, type_min_size[Type::FLOAT]);
            return float_;
        }
        case Type::DECIMAL: {
            void *decimal = malloc(type_min_size[Type::DECIMAL]);
            memcpy(decimal, &value, type_min_size[Type::DECIMAL]);
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
        case Type::BOOLEAN:
            return new bool(std::stoi(value));
        case Type::BIT:
            return parseBit(value, size);
        case Type::BLOB:
        case Type::VARBINARY:
            return parseBinary(value, size);
    }
    return nullptr;
}

void *parseDate(std::string &value) {
    void *date = malloc(type_min_size[Type::DATE]);
    std::string year = value.substr(0, 4);
    std::string month = value.substr(5, 2);
    std::string day = value.substr(8, 2);
    // Count day since 1900-01-01
    int days = (std::stoul(year) - 1900) * 365 + (std::stoul(month) - 1) * 30 +
               std::stoul(day);
    memcpy(date, &days, 3);
    return date;
}

void *parseTime(std::string &value) {
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

void *parseTimestamp(std::string &value) {
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

void *parseBit(std::string &value, uint64_t size) {
    void *bit = malloc(size / 8 + 1);
    memset(bit, 0, size / 8 + 1);
    for (int i = 0; i < size; ++i)
        if (value[i] == '1') ((char *)bit)[i / 8] |= (1 << (i % 8));
    return bit;
}

void *parseBinary(std::string &value, uint64_t size) {
    void *binary = malloc(size);
    memcpy(binary, value.data(), size);
    return binary;
}

bool isCorrectType(std::string &value, Type type) {
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
            try {
                std::stoi(value);
            } catch (const std::exception &e) {
                return false;
            }
            return true;
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

bool isCorrectDate(std::string &value) {
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

bool isCorrectTime(std::string &value) {
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

bool isCorrectTimestamp(std::string &value) {
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

__uint128_t getTypeSize(std::string &value, Type type) {
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
            return (value.size() / 8) + 1;
        case Type::BLOB:
            return value.size();
        case Type::VARBINARY:
            return value.size();
        default:
            return 0;
    }
    return 0;
}
}  // namespace utils