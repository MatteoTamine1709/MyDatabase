#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <unordered_map>
#include <vector>

#include "Constant.h"
#include "Type.h"

namespace utils {
void *parseType(std::string &value, Type type);
void *parseDate(std::string &value);
void *parseTime(std::string &value);
void *parseTimestamp(std::string &value);
void *parseBit(std::string &value, uint64_t size);
void *parseBinary(std::string &value, uint64_t size);

bool isCorrectType(std::string &value, Type type);
bool isCorrectDate(std::string &value);
bool isCorrectTime(std::string &value);
bool isCorrectTimestamp(std::string &value);

__uint128_t getTypeSize(std::string &value, Type type);
}  // namespace utils

#endif