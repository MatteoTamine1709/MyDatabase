#ifndef TABLE_H
#define TABLE_H

#include <string.h>

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include "BTree.h"
#include "Condition.h"
#include "Constant.h"
#include "Index.h"
#include "Key.h"
#include "Row.h"
#include "Type.h"

class Table {
   public:
    Table(std::string filepath) { this->load(filepath); };
    Table(std::string name, std::vector<std::string> columns_name,
          std::vector<Type> columns_type, std::string primary_key_column,
          std::vector<bool> is_unique, std::vector<bool> is_not_null,
          std::vector<std::string> default_values);
    ~Table();

    std::string insert(std::vector<std::string> column_order,
                       std::vector<std::string> &values);
    std::pair<std::string, void *> select(
        std::vector<std::string> requestedColumns,
        std::vector<Condition> conditions);
    std::string update(std::string column, std::string value,
                       std::vector<Condition> conditions);
    std::string delete_(std::vector<Condition> conditions);
    std::string createIndex(const std::vector<std::string> &column_names);
    std::string dropIndex(std::string index_name);
    void prettyPrint(Index idx) const {
        std::cout << "Table name: " << name << std::endl;
        std::cout << "Column names: ";
        for (auto column_name : this->column_names)
            std::cout << column_name << " ";
        std::cout << std::endl;
        std::cout << "Column types: ";
        for (auto column_type : this->column_types)
            std::cout << type_name[column_type] << " ";
        std::cout << std::endl;
        std::cout << "Primary key column: " << primary_key_column << std::endl;
        std::cout << "Is unique: ";
        for (auto is_unique : this->is_unique) std::cout << is_unique << " ";
        std::cout << std::endl;
        std::cout << "Is not null: ";
        for (auto is_not_null : this->is_not_null)
            std::cout << is_not_null << " ";
        std::cout << std::endl;
        std::cout << "Default value: ";
        for (auto default_value : this->default_values)
            std::cout << default_value << " ";
        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << "Indexes: ";
        for (auto index : indexes) std::cout << index.first.toString() << " ";
        std::cout << std::endl;

        if (indexes.find(idx) == indexes.end())
            std::cout << "Index not found" << std::endl;
        else
            indexes.at(idx)->prettyPrint();
    };
    void save(std::filesystem::path dbFolderPath);
    void load(std::filesystem::path dbFolderPath);

   private:
    std::string name;
    std::vector<std::string> column_names;
    std::vector<Type> column_types;
    std::unordered_map<int64_t, std::shared_ptr<Row>> rows;
    int64_t next_row_idx = 0;
    std::unordered_map<Index, BTree *, Index::hashFn> indexes;
    std::vector<bool> is_unique;
    std::vector<bool> is_not_null;
    std::vector<std::string> default_values;
    std::string primary_key_column;

    size_t getColumnIndex(std::string column_name);
};

#endif
