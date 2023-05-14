#include "Database.h"

Database::Database(std::string name)
    : name(name), tables(std::unordered_map<std::string, Table *>()) {}

Database::~Database() {
    for (auto table : tables) delete table.second;
}

std::string Database::showTables() {
    std::string str = "";
    for (auto table : tables) str += table.first + "\n";
    return str;
}

std::string Database::createTable(
    std::string name, std::vector<std::pair<std::string, Type>> columns,
    std::string primary_key_column, std::vector<bool> is_unique,
    std::vector<bool> is_not_null, std::vector<std::string> default_value) {
    if (tables.find(name) != tables.end()) return "Error: Table already exists";
    std::vector<std::string> column_names;
    std::vector<Type> column_types;
    for (int i = 0; i < columns.size(); ++i) {
        column_names.push_back(columns[i].first);
        column_types.push_back(columns[i].second);
    }
    Table *table =
        new Table(name, column_names, column_types, primary_key_column,
                  is_unique, is_not_null, default_value);
    tables[name] = table;
    return "Table created";
}

std::string Database::dropTable(std::string name) {
    if (tables.find(name) == tables.end()) return "Error: Table not found";
    delete tables[name];
    tables.erase(name);
    return "Table dropped";
}

std::string Database::insert(std::string table_name,
                             std::vector<std::string> column_order,
                             std::vector<std::string> values) {
    if (tables.find(table_name) == tables.end())
        return "Error: Table not found";
    return tables[table_name]->insert(column_order, values);
}

std::pair<std::string, void *> Database::select(
    std::string table_name, std::vector<std::string> columns,
    std::vector<Condition> conditions) {
    if (tables.find(table_name) == tables.end())
        return {"Error: Table not found", {}};
    return tables[table_name]->select(columns, conditions);
}