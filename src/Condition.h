#ifndef CONDITION_H
#define CONDITION_H

#include <string>
#include <unordered_map>
#include <vector>

#include "Constant.h"
#include "Type.h"

class Condition {
   public:
    Condition(std::string column, std::string value, std::string op);
    ~Condition();

    bool check(std::unordered_map<std::string, Type> columns,
               std::vector<std::string> values);

   private:
    std::string column;
    std::string value;
    std::string op;
};

#endif