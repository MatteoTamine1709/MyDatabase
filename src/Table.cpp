#include "Table.h"

#include <string.h>

#include <iostream>

#include "utils.h"

Table::Table(std::string name, std::vector<std::string> columns_name,
             std::vector<Type> columns_type, std::string primary_key_column,
             std::vector<bool> is_unique, std::vector<bool> is_not_null,
             std::vector<std::string> default_value) {
    this->name = name;
    this->column_names = columns_name;
    this->column_types = columns_type;
    this->primary_key_column = primary_key_column;
    this->is_unique = is_unique;
    if (this->is_unique.size() == 0)
        this->is_unique = std::vector<bool>(columns_name.size(), false);
    this->is_not_null = is_not_null;
    if (this->is_not_null.size() == 0)
        this->is_not_null = std::vector<bool>(columns_name.size(), false);
    this->default_value = default_value;
    if (this->default_value.size() == 0)
        this->default_value = std::vector<std::string>(columns_name.size(), "");
    if (this->primary_key_column != "")
        this->indexes[Index(this->name, this->primary_key_column)] =
            new BTree<void *, Row>();
    else
        this->indexes[Index(this->name, this->column_names[0])] =
            new BTree<void *, Row>();
    for (int i = 0; i < columns_name.size(); ++i)
        if (this->is_unique[i])
            this->indexes[Index(this->name, this->column_names[i])] =
                new BTree<void *, Row>();
}

Table::~Table() {
    for (auto &[index, btree] : this->indexes) delete btree;
}

std::string Table::insert(std::vector<std::string> column_order,
                          std::vector<std::string> &values) {
    if (column_order.size() != values.size())
        return "Error: Number of columns and values don't match";
    for (int i = 0; i < column_order.size(); ++i) {
        bool found = false;
        for (int j = 0; j < this->column_names.size(); ++j)
            if (column_order[i] == this->column_names[j]) {
                found = true;
                break;
            }
        if (!found) return "Error: Column not found";
    }

    std::vector<char> is_set(this->column_names.size(), true);
    for (int i = 0; i < values.size(); ++i) {
        if (values[i] == "") {
            if (this->is_not_null[i])
                return "Error: Not null constraint violated";
            if (this->default_value[i] == "") is_set[i] = false;
            values[i] = this->default_value[i];
        } else {
            if (utils::isCorrectType(values[i], column_types[i])) {
                if (column_types[i] == Type::CHAR ||
                    column_types[i] == Type::VARCHAR ||
                    column_types[i] == Type::TEXT)
                    if (values[i].size() > type_max_size[column_types[i]])
                        return "Error: Size of value is too big";
            } else
                return "Error: Invalid type of value";
        }
    }

    uint64_t row_size = 0;
    std::vector<uint64_t> sizes;
    for (int i = 0; i < values.size(); ++i) {
        row_size += utils::getTypeSize(values[i], column_types[i]);
        sizes.push_back(utils::getTypeSize(values[i], column_types[i]));
    }

    void *row = malloc(row_size);
    uint64_t offset = 0;

    for (int i = 0; i < values.size(); ++i) {
        void *value = utils::parseType(values[i], column_types[i]);
        memcpy((char *)row + offset, value,
               utils::getTypeSize(values[i], column_types[i]));
        offset += utils::getTypeSize(values[i], column_types[i]);
        free(value);
    }

    if (this->primary_key_column != "") {
        int primary_key_column_index =
            this->getColumnIndex(this->primary_key_column);
        if (primary_key_column_index == -1)
            return "Error: Primary key not found";

        void *primary_key =
            utils::parseType(values[primary_key_column_index],
                             column_types[primary_key_column_index]);
        if (this->indexes[Index(this->name, this->primary_key_column)]
                ->search(primary_key)
                .first != nullptr)
            return "Error: Primary key constraint violated";
        free(primary_key);
    }

    for (int i = 0; i < this->column_names.size(); ++i) {
        if (this->is_unique[i]) {
            void *value = utils::parseType(values[i], column_types[i]);
            if (this->indexes[Index(this->name, this->column_names[i])]
                    ->search(value)
                    .first != nullptr) {
                free(value);
                return "Error: Unique constraint violated";
            }
            free(value);
        }
    }

    for (const auto &[index, btree] : this->indexes) {
        void *key = utils::parseType(
            values[this->getColumnIndex(index.columns_name[0])],
            this->column_types[this->getColumnIndex(index.columns_name[0])]);
        Row r = Row(row, sizes, is_set);
        this->rows.push_back(r);
        btree->insert(key, std::move(r));
        free(key);
    }
    return "Row inserted";
}

size_t Table::getColumnIndex(std::string column_name) {
    for (int i = 0; i < this->column_names.size(); ++i)
        if (this->column_names[i] == column_name) return i;
    return -1;
}

std::pair<std::string, void *> Table::select(
    std::vector<std::string> columns, std::vector<Condition> conditions) {
    if (columns.size() == 0) columns = this->column_names;
    for (int i = 0; i < columns.size(); ++i)
        if (this->getColumnIndex(columns[i]) == -1)
            return {"Error: Column not found", {}};

    uint64_t total_size = sizeof(size_t) + sizeof(size_t);
    for (int i = 0; i < columns.size(); ++i) total_size += sizeof(char);
    for (const auto &row : this->rows) total_size += row.totalSize;
    void *rows = malloc(total_size);
    uint64_t offset = 0;
    size_t rowCount = this->rows.size();
    memcpy((char *)rows + offset, &rowCount, sizeof(size_t));
    offset += sizeof(size_t);
    size_t column_count = columns.size();
    memcpy((char *)rows + offset, &column_count, sizeof(size_t));
    offset += sizeof(size_t);
    for (int i = 0; i < columns.size(); ++i) {
        int type = (int)this->column_types[this->getColumnIndex(columns[i])];
        memcpy((char *)rows + offset, &type, sizeof(char));
        offset += sizeof(char);
    }

    for (const auto &row : this->rows) {
        void *b = row.blob();
        memcpy((char *)rows + offset, b, row.totalSize);
        free(b);
        offset += row.totalSize;
    }
    return {std::string("Rows selected"), rows};
}