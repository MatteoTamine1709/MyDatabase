#ifndef INDEX_H
#define INDEX_H

#include <iostream>
#include <string>
#include <vector>

#include "BTree.h"
#include "Constant.h"
#include "Type.h"

struct Index {
    Index(std::string table, std::vector<std::string> columns_name)
        : table(table), columns_name(columns_name) {}
    Index(std::string table, std::string column_name)
        : table(table), columns_name({column_name}) {}
    ~Index() = default;

    struct hashFn {
        size_t operator()(const Index &index) const {
            size_t hash = 0;
            for (int i = 0; i < index.columns_name.size(); i++)
                hash =
                    hash * 31 + std::hash<std::string>()(index.columns_name[i]);
            return hash;
        }
    };

    // comparison function
    bool operator==(const Index &index) const {
        if (columns_name.size() != index.columns_name.size()) return false;
        for (int i = 0; i < columns_name.size(); i++)
            if (columns_name[i] != index.columns_name[i]) return false;
        return true;
    }

    operator std::string() const {
        std::string str = table + "(";
        for (int i = 0; i < columns_name.size(); i++) {
            str += columns_name[i];
            if (i != columns_name.size() - 1) str += ", ";
        }
        str += ")";
        return str;
    }

    std::string toString() const {
        std::string str = table + "(";
        for (int i = 0; i < columns_name.size(); i++) {
            str += columns_name[i];
            if (i != columns_name.size() - 1) str += ", ";
        }
        str += ")";
        return str;
    }

    std::string table;
    std::vector<std::string> columns_name;
};

#endif