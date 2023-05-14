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

struct Row {
    std::shared_ptr<void> data;
    Row(void *data) { this->data = std::shared_ptr<void>(data, free); }

    ~Row() {}
};
class Table {
   public:
    Table(std::string name, std::vector<std::string> columns_name,
          std::vector<Type> columns_type, std::string primary_key_column,
          std::vector<bool> is_unique, std::vector<bool> is_not_null,
          std::vector<std::string> default_value);
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
        std::cout << "Is set: ";
        for (auto is_set : is_set) std::cout << is_set << " ";
        std::cout << std::endl;
        std::cout << "Row sizes: ";
        for (auto row_size : row_sizes) std::cout << row_size << " ";
        std::cout << std::endl;
        std::cout << "Indexes: ";
        for (auto index : indexes) std::cout << index.first.toString() << " ";
        std::cout << std::endl;
    };
    void save();
    void load();

   private:
    std::string name;
    std::vector<std::string> column_names;
    std::vector<Type> column_types;
    std::vector<void *> rows;
    std::vector<uint64_t> row_sizes;
    std::unordered_map<Index, BTree<void *, Row> *, Index::hashFn> indexes;
    std::vector<bool> is_unique;
    std::vector<bool> is_not_null;
    std::vector<std::string> default_value;
    std::string primary_key_column;
    std::vector<bool> is_set;

    size_t getColumnIndex(std::string column_name);
};

#endif
