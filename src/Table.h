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
    Table(std::string filepath) { load(filepath); };
    Table(std::string name, std::vector<std::string> columns_name,
          std::vector<Type> columns_type, std::string primary_key_column,
          std::vector<bool> is_unique, std::vector<bool> is_not_null,
          std::vector<std::string> default_value);
    ~Table();

    std::string insert(std::vector<std::string> column_order,
                       std::vector<std::string> &values);
    std::pair<std::string, void *> select(
        std::vector<std::string> requestedColumns,
        std::vector<Condition> conditions);
    std::string update(std::string column, std::string value,
                       std::vector<Condition> conditions);
    std::string delete_(std::vector<Condition> conditions);
    std::string createIndex(std::string index_name,
                            std::vector<std::string> column_name);
    std::string dropIndex(std::string index_name);
    void prettyPrint() const {
        std::cout << "Table name: " << name << std::endl;
        std::cout << "Column names: ";
        for (auto column_name : column_names) std::cout << column_name << " ";
        std::cout << std::endl;
        std::cout << "Column types: ";
        for (auto column_type : column_types)
            std::cout << type_name[column_type] << " ";
        std::cout << std::endl;
        std::cout << "Primary key column: " << primary_key_column << std::endl;
        std::cout << "Is unique: ";
        for (auto is_unique : is_unique) std::cout << is_unique << " ";
        std::cout << std::endl;
        std::cout << "Is not null: ";
        for (auto is_not_null : is_not_null) std::cout << is_not_null << " ";
        std::cout << std::endl;
        std::cout << "Default value: ";
        for (auto default_value : default_value)
            std::cout << default_value << " ";
        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << "Indexes: ";
        for (auto index : indexes) std::cout << index.first.toString() << " ";
        std::cout << std::endl;

        if (this->primary_key_column != "")
            indexes.at(Index(this->name, this->primary_key_column))
                ->prettyPrint();
        else
            indexes.at(Index(this->name, this->column_names[0]))->prettyPrint();
    };
    void save(std::filesystem::path dbFolderPath);
    void load(std::filesystem::path dbFolderPath);

   private:
    std::string name;
    std::vector<std::string> column_names;
    std::vector<Type> column_types;
    std::vector<Row> rows;
    std::unordered_map<Index, BTree *, Index::hashFn> indexes;
    std::vector<bool> is_unique;
    std::vector<bool> is_not_null;
    std::vector<std::string> default_value = {"NULL"};
    std::string primary_key_column;

    size_t getColumnIndex(std::string column_name);
};

#endif
