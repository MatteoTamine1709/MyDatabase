#ifndef TABLE_H
#define TABLE_H

#include <string>
#include <unordered_map>
#include <vector>

#include "BTree.h"
#include "Condition.h"
#include "Constant.h"
#include "Index.h"
#include "Type.h"

class Table {
   public:
    Table(std::string name, std::vector<std::string> columns_name,
          std::vector<Type> columns_type);
    ~Table();

    std::string insert(std::vector<std::string> column_order,
                       std::vector<std::string> &values);
    std::string select(std::vector<std::string> columns,
                       std::vector<Condition> conditions);
    std::string update(std::string column, std::string value,
                       std::vector<Condition> conditions);
    std::string delete_(std::vector<Condition> conditions);
    std::string createIndex(std::string index_name,
                            std::vector<std::string> column_name);
    std::string dropIndex(std::string index_name);
    void prettyPrint();
    void save();
    void load();

   private:
    std::string name;
    std::vector<std::string> column_names;
    std::vector<Type> column_types;
    std::vector<void *> rows;
    std::vector<unsigned __int128> row_sizes;
    std::unordered_map<Index, BTree<void *, void *> *, Index::hashFn> indexes;
    std::vector<bool> is_unique;
    std::vector<bool> is_not_null;
    std::vector<std::string> default_value;
    std::string primary_key_column;
    std::vector<bool> is_set;

    size_t getColumnIndex(std::string column_name);
};

#endif
