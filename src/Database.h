#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <unordered_map>
#include <vector>

#include "Constant.h"
#include "Table.h"
#include "Type.h"

class Database {
   public:
    Database(std::string name);
    ~Database();

    std::string showTables();
    std::string createTable(
        std::string name, std::vector<std::pair<std::string, Type>> columns,
        std::string primary_key_column = "",
        std::vector<bool> is_unique = std::vector<bool>(),
        std::vector<bool> is_not_null = std::vector<bool>(),
        std::vector<std::string> default_value = std::vector<std::string>());
    std::string dropTable(std::string name);
    std::string insert(std::string table_name,
                       std::vector<std::string> column_order,
                       std::vector<std::string> values);
    std::string createView(std::string view_name, std::string select_statement);
    std::pair<std::string, void *> select(std::string table_name,
                                          std::vector<std::string> columns,
                                          std::vector<Condition> conditions);
    std::string update(std::string table_name, std::string column,
                       std::string value, std::string where_column,
                       std::string where_value);
    std::string delete_(std::string table_name, std::string column,
                        std::string value);
    std::string createIndex(std::string table_name, std::string index_name,
                            std::vector<std::string> column_name);
    std::string dropIndex(std::string table_name, std::string index_name);
    std::string query(std::string query);
    void prettyPrint(std::string table) const {
        if (tables.find(table) == tables.end())
            std::cout << "Table not found" << std::endl;
        else
            tables.at(table)->prettyPrint();
    }
    void save();
    void load();

   private:
    std::string name;
    std::unordered_map<std::string, Table *> tables;
    std::unordered_map<std::string, Table *> views;
};

#endif