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
    this->is_set = std::vector<bool>(columns_name.size(), false);
    this->row_sizes = std::vector<uint64_t>();
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
    std::cout << "Table destructor" << std::endl;
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

    for (int i = 0; i < values.size(); ++i) {
        if (values[i] == "") {
            if (this->is_not_null[i])
                return "Error: Not null constraint violated";
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
    for (int i = 0; i < values.size(); ++i)
        row_size += utils::getTypeSize(values[i], column_types[i]);

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

    this->rows.push_back(row);
    this->row_sizes.push_back(row_size);
    for (const auto &[index, btree] : this->indexes) {
        void *key = utils::parseType(
            values[this->getColumnIndex(index.columns_name[0])],
            this->column_types[this->getColumnIndex(index.columns_name[0])]);
        std::cout << "Inserting " << index.toString() << " " << key << " "
                  << row << std::endl;
        btree->insert(key, std::move(row));
        free(key);
    }
    return "Row inserted";
}

size_t Table::getColumnIndex(std::string column_name) {
    for (int i = 0; i < this->column_names.size(); ++i)
        if (this->column_names[i] == column_name) return i;
    return -1;
}