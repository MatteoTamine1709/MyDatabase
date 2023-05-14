#ifndef TYPE_H
#define TYPE_H

#include <cstdint>
#include <string>
#include <vector>

#include "Constant.h"

enum Type {
    INTEGER,
    FLOAT,
    DECIMAL,
    CHAR,
    VARCHAR,
    TEXT,
    DATE,
    TIME,
    TIMESTAMP,
    BOOLEAN,
    BIT,
    BLOB,
    VARBINARY
};

extern std::vector<uint64_t> type_min_size;
extern std::vector<uint64_t> type_max_size;
extern std::vector<std::string> type_name;

#endif