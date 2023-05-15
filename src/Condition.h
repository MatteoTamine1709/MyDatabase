#ifndef CONDITION_H
#define CONDITION_H

#include <string>
#include <unordered_map>
#include <vector>

#include "Constant.h"
#include "Key.h"
#include "Type.h"
#include "utils.h"

struct Condition {
    Condition(Type type, std::string value, std::string op) {
        this->type = type;
        this->value = value;
        this->op = op;
    }
    ~Condition(){};

    std::vector<std::pair<Key, Key>> generateRanges() const {
        if (op == "=") {
            return {{Key(utils::parseType(value, type), type_max_size[type]),
                     Key(utils::parseType(value, type), type_max_size[type])}};
        }
        if (op == "!=") {
            return {{Key(nullptr, 0),
                     Key(utils::parseType(value, type), type_max_size[type])},
                    {Key(utils::parseType(value, type), type_max_size[type]),
                     Key(nullptr, 0)}};
        }
        if (op == "<=" || op == "<") {
            return {{Key(nullptr, 0),
                     Key(utils::parseType(value, type), type_max_size[type])}};
        }
        if (op == ">=" || op == ">") {
            return {{Key(utils::parseType(value, type), type_max_size[type]),
                     Key(nullptr, 0)}};
        }
        return {{Key(nullptr, 0), Key(nullptr, 0)}};
    }

    Type type;
    std::string value;
    std::string op;
};

#endif