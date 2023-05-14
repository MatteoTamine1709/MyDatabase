#include "Type.h"

#include <limits.h>

std::vector<uint64_t> type_min_size = {
    4,  // INTEGER
    4,  // FLOAT
    0,  // DECIMAL
    0,  // CHAR
    0,  // VARCHAR
    0,  // TEXT
    3,  // DATE
    5,  // TIME
    8,  // TIMESTAMP
    1,  // BOOLEAN
    0,  // BIT
    0,  // BLOB
    0   // VARBINARY
};

std::vector<uint64_t> type_max_size = {
    4,                               // INTEGER
    4,                               // FLOAT
    UINT_MAX,                        // DECIMAL
    UINT_MAX,                        // CHAR
    UINT_MAX,                        // VARCHAR
    UINT_MAX,                        // TEXT
    3,                               // DATE
    5,                               // TIME
    8,                               // TIMESTAMP
    1,                               // BOOLEAN
    UINT_MAX,                        // BIT
    (uint64_t)2 * (uint64_t)ONE_GB,  // BLOB
    8000                             // VARBINARY
};

std::vector<std::string> type_name = {
    "INTEGER", "FLOAT",     "DECIMAL", "CHAR", "VARCHAR", "TEXT",     "DATE",
    "TIME",    "TIMESTAMP", "BOOLEAN", "BIT",  "BLOB",    "VARBINARY"};