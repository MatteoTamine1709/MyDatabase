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
        this->default_value =
            std::vector<std::string>(columns_name.size(), "NULL");
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

    values.resize(this->column_names.size(), "");

    // Reorder values to match column_names
    std::vector<std::string> new_values(this->column_names.size(), "");
    for (int i = 0; i < column_order.size(); ++i) {
        int index = this->getColumnIndex(column_order[i]);
        new_values[index] = values[i];
    }
    values = new_values;

    std::vector<char> is_set(this->column_names.size(), true);
    for (int i = 0; i < values.size(); ++i) {
        if (values[i] == "") {
            values[i] = this->default_value[i];
        }
        if (values[i] == "NULL") {
            is_set[i] = false;
            if (this->is_not_null[i])
                return "Error: Not null constraint violated";
        } else {
            std::cout << values[i] << ", " << column_types[i] << std::endl;
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

    // std::cout << "Row size: " << row_size << std::endl;

    void *row = malloc(row_size);
    uint64_t offset = 0;

    for (int i = 0; i < values.size(); ++i) {
        // std::cout << values[i] << ", " << sizes[i] << ", " << offset
        //           << std::endl;
        void *value = utils::parseType(values[i], column_types[i]);
        if (value == nullptr) {
            memset((char *)row + offset, 0, sizes[i]);
        } else {
            memcpy((char *)row + offset, value,
                   utils::getTypeSize(values[i], column_types[i]));
        }
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
        // size_t offset = 0;
        // for (int i = 0; i < this->column_names.size(); ++i) {
        //     std::cout << this->column_names[i] << ": " << values[i] << ", "
        //               << sizes[i] << ", " << (int)is_set[i] << std::endl;
        //     std::cout << "Rowv: "
        //               << utils::getValue(this->column_types[i],
        //                                  (char *)row + offset, sizes[i])
        //               << std::endl;
        //     offset += sizes[i];
        // }
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
    std::vector<std::string> requestedColumns,
    std::vector<Condition> conditions) {
    if (requestedColumns.size() == 0) requestedColumns = this->column_names;
    for (int i = 0; i < requestedColumns.size(); ++i)
        if (this->getColumnIndex(requestedColumns[i]) == -1)
            return {"Error: Column not found", {}};

    std::vector<int> requestedColumnIndexes;
    for (int i = 0; i < requestedColumns.size(); ++i)
        requestedColumnIndexes.push_back(
            this->getColumnIndex(requestedColumns[i]));

    for (int i = 0; i < requestedColumns.size(); ++i)
        std::cout << requestedColumns[i] << " ";
    std::cout << std::endl;
    // number of rows, number of columns
    uint64_t headerSize = sizeof(size_t) + sizeof(size_t);
    uint64_t total_size = 0;
    // column name
    for (int i = 0; i < requestedColumns.size(); ++i)
        headerSize += requestedColumns[i].size() + 1;
    // column type
    for (int i = 0; i < requestedColumns.size(); ++i)
        headerSize += sizeof(char);
    total_size += headerSize;
    for (auto &row : this->rows)
        total_size += row.computeTotalSize(requestedColumnIndexes);
    std::cout << "Total size: " << total_size << std::endl;
    std::cout << "Header size: " << headerSize << std::endl;
    void *rows = malloc(total_size);
    uint64_t offset = 0;
    size_t rowCount = this->rows.size();
    memcpy((char *)rows + offset, &rowCount, sizeof(size_t));
    offset += sizeof(size_t);
    size_t column_count = requestedColumns.size();
    memcpy((char *)rows + offset, &column_count, sizeof(size_t));
    offset += sizeof(size_t);
    for (int i = 0; i < requestedColumns.size(); ++i) {
        memcpy((char *)rows + offset, requestedColumns[i].c_str(),
               requestedColumns[i].size() + 1);
        offset += requestedColumns[i].size() + 1;
    }
    for (int i = 0; i < requestedColumns.size(); ++i) {
        int type =
            (int)this->column_types[this->getColumnIndex(requestedColumns[i])];
        memcpy((char *)rows + offset, &type, sizeof(char));
        offset += sizeof(char);
    }
    for (auto &row : this->rows) {
        void *b = row.blob(requestedColumnIndexes);
        std::cout << "Row size: " << row.totalSize << std::endl;
        memcpy((char *)rows + offset, b,
               row.computeTotalSize(requestedColumnIndexes));
        // size_t offset2 = 0;
        // for (int i = 0; i < this->column_names.size(); ++i) {
        //     std::cout << "Is set: "
        //               << (int)*((char *)b +
        //                         (row.sizes.size() * sizeof(uint64_t)) + i)
        //               << std::endl;
        //     std::cout << this->column_names[i] << ": "
        //               << utils::getValue(this->column_types[i],
        //                                  (char *)b + row.headerSize +
        //                                  offset2, row.sizes[i])
        //               << std::endl;
        //     offset2 += row.sizes[i];
        // }
        free(b);
        offset += row.totalSize;
    }
    return {std::string("Rows selected"), rows};
}