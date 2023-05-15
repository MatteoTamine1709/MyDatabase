#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <unordered_map>
#include <vector>

#include "Constant.h"
#include "Type.h"

namespace utils {
void *parseType(const std::string &value, Type type);
void *parseDate(const std::string &value);
void *parseTime(const std::string &value);
void *parseTimestamp(const std::string &value);
void *parseBit(const std::string &value);
void *parseBinary(const std::string &value);

bool isCorrectType(const std::string &value, Type type);
bool isCorrectDate(const std::string &value);
bool isCorrectTime(const std::string &value);
bool isCorrectTimestamp(const std::string &value);

uint64_t getTypeSize(const std::string &value, Type type);

std::string getValue(char type, void *value, uint64_t size);
void displaySelection(void *blob);

int reverse_memcmp(const void *s1, const void *s2, size_t n);
}  // namespace utils

#endif