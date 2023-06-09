#include "Table.h"

#include <string.h>

#include <algorithm>
#include <fstream>
#include <iostream>

#include "utils.h"

Table::Table(std::string name, std::vector<std::string> columns_name,
             std::vector<Type> columns_type, std::string primary_key_column,
             std::vector<bool> is_unique, std::vector<bool> is_not_null,
             std::vector<std::string> default_values) {
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
    this->default_values = default_values;
    if (this->default_values.size() == 0)
        this->default_values =
            std::vector<std::string>(columns_name.size(), "NULL");
    if (this->primary_key_column != "")
        this->indexes[Index(this->name, this->primary_key_column)] =
            new BTree();
    else
        this->indexes[Index(this->name, this->column_names[0])] = new BTree();
    for (int i = 0; i < columns_name.size(); ++i)
        if (this->is_unique[i])
            this->indexes[Index(this->name, this->column_names[i])] =
                new BTree();
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
            values[i] = this->default_values[i];
        }
        if (values[i] == "NULL") {
            is_set[i] = false;
            if (this->is_not_null[i])
                return "Error: Not null constraint violated";
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
                ->search(Key(
                    utils::parseType(values[primary_key_column_index],
                                     column_types[primary_key_column_index]),
                    column_types[primary_key_column_index]))
                .first != nullptr)
            return "Error: Primary key constraint violated";
        free(primary_key);
    }

    for (int i = 0; i < this->column_names.size(); ++i) {
        if (this->is_unique[i]) {
            void *value = utils::parseType(values[i], column_types[i]);
            if (this->indexes[Index(this->name, this->column_names[i])]
                    ->search(Key(utils::parseType(values[i], column_types[i]),
                                 column_types[i]))
                    .first != nullptr) {
                free(value);
                return "Error: Unique constraint violated";
            }
            free(value);
        }
    }

    this->rows[this->next_row_idx] = std::make_shared<Row>(row, sizes, is_set);
    for (const auto &[index, btree] : this->indexes) {
        void *key = utils::parseType(
            values[this->getColumnIndex(index.columns_name[0])],
            this->column_types[this->getColumnIndex(index.columns_name[0])]);
        btree->insert(Key(key, this->column_types[this->getColumnIndex(
                                   index.columns_name[0])]),
                      this->rows.size() - 1);
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
    // Experiment
    auto range = indexes[Index(this->name, this->column_names[0])]->searchRange(
        conditions);
    if (requestedColumns.size() == 0) requestedColumns = this->column_names;
    for (int i = 0; i < requestedColumns.size(); ++i)
        if (this->getColumnIndex(requestedColumns[i]) == -1)
            return {"Error: Column not found", {}};

    std::vector<int> requestedColumnIndexes;
    for (int i = 0; i < requestedColumns.size(); ++i)
        requestedColumnIndexes.push_back(
            this->getColumnIndex(requestedColumns[i]));
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
    for (auto &[key, rowIdx] : range) {
        total_size +=
            this->rows[rowIdx]->computeTotalSize(requestedColumnIndexes);
    }
    void *rows = malloc(total_size);
    uint64_t offset = 0;
    size_t rowCount = range.size();
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
    for (auto &[key, rowIdx] : range) {
        void *b = this->rows[rowIdx]->blob(requestedColumnIndexes);
        memcpy((char *)rows + offset, b,
               this->rows[rowIdx]->computeTotalSize(requestedColumnIndexes));
        free(b);
        offset += this->rows[rowIdx]->totalSize;
    }
    return {std::string("OK"), rows};
}

std::string Table::createIndex(const std::vector<std::string> &column_names) {
    for (int i = 0; i < column_names.size(); ++i)
        if (this->getColumnIndex(column_names[i]) == -1)
            return "Error: Column not found";
    if (column_names.size() == 0) return "Error: No column specified";
    if (column_names.size() > 1) return "Error: Only one column allowed";
    if (this->indexes.find(Index(this->name, column_names[0])) !=
        this->indexes.end())
        return "Error: Index already exists";
    this->indexes[Index(this->name, column_names[0])] = new BTree();
    std::cout << "Index type "
              << this->column_types[this->getColumnIndex(column_names[0])]
              << std::endl;
    for (int64_t i = 0; i < this->rows.size(); ++i) {
        void *key = this->rows[i]->getKey(
            this->getColumnIndex(column_names[0]),
            this->column_types[this->getColumnIndex(column_names[0])]);
        this->indexes[Index(this->name, column_names[0])]->insert(
            Key(key, this->column_types[this->getColumnIndex(column_names[0])]),
            i);
    }
    return "Index created";
}

void Table::save(std::filesystem::path dbFolderPath) {
    std::string tableFolderPath = dbFolderPath.string() + "/" + this->name;
    std::cout << "Saving table " << tableFolderPath << std::endl;
    std::filesystem::create_directories(tableFolderPath);
    char tableInfoBlob[MAX_PAGE_SIZE] = {0};
    uint64_t offset = 0;
    // Table name
    memcpy(tableInfoBlob + offset, this->name.c_str(), this->name.size() + 1);
    offset += this->name.size() + 1;
    // Column names
    size_t column_names_size = this->column_names.size();
    memcpy(tableInfoBlob + offset, &column_names_size, sizeof(size_t));
    offset += sizeof(size_t);
    for (int i = 0; i < this->column_names.size(); ++i) {
        memcpy(tableInfoBlob + offset, this->column_names[i].c_str(),
               this->column_names[i].size() + 1);
        offset += this->column_names[i].size() + 1;
    }
    // Column types
    for (int i = 0; i < this->column_types.size(); ++i) {
        memcpy(tableInfoBlob + offset, &this->column_types[i], sizeof(char));
        offset += sizeof(char);
    }
    // Next row index
    memcpy(tableInfoBlob + offset, &this->next_row_idx, sizeof(int64_t));
    offset += sizeof(int64_t);
    // Primary key column
    memcpy(tableInfoBlob + offset, this->primary_key_column.c_str(),
           this->primary_key_column.size() + 1);
    offset += this->primary_key_column.size() + 1;
    // Is unique
    for (int i = 0; i < this->is_unique.size(); ++i) {
        bool temp = this->is_unique[i];
        memcpy(tableInfoBlob + offset, &temp, sizeof(bool));
        offset += sizeof(bool);
    }
    // Is not null
    for (int i = 0; i < this->is_not_null.size(); ++i) {
        bool temp = this->is_not_null[i];
        memcpy(tableInfoBlob + offset, &temp, sizeof(bool));
        offset += sizeof(bool);
    }
    // Default value
    for (int i = 0; i < this->default_values.size(); ++i) {
        memcpy(tableInfoBlob + offset, this->default_values[i].c_str(),
               this->default_values[i].size() + 1);
        offset += this->default_values[i].size() + 1;
    }

    size_t rows_size = this->rows.size();
    memcpy(tableInfoBlob + offset, &rows_size, sizeof(size_t));
    offset += sizeof(size_t);

    // Indexes name in tableInfoBlob
    for (auto &[index, btree] : this->indexes) {
        memcpy(tableInfoBlob + offset, index.toString().c_str(),
               index.toString().size() + 1);
        offset += index.toString().size() + 1;
    }

    std::ofstream tableInfoFile(tableFolderPath + "/tableInfo",
                                std::ios::binary);
    tableInfoFile.write(tableInfoBlob, MAX_PAGE_SIZE);
    tableInfoFile.close();

    std::filesystem::create_directories(tableFolderPath + "/rows");
    // Save rows
    // int index = 0;
    // for (auto &row : this->rows) {
    //     std::ofstream file(tableFolderPath + "/rows/" +
    //     std::to_string(index),
    //                        std::ios::binary);
    //     row->save(file);
    //     file.close();
    //     ++index;
    // }

    // Save indexes
    for (auto &[index, btree] : this->indexes) {
        std::cout << "Saving index " << index.toString() << std::endl;
        btree->save(tableFolderPath + "/" + index.toString());
    }
}

void Table::load(std::filesystem::path dbFolderPath) {
    std::cout << "Loading table " << dbFolderPath.string() << std::endl;
    std::ifstream tableInfoFile(dbFolderPath.string() + "/tableInfo",
                                std::ios::binary);
    char tableInfoBlob[MAX_PAGE_SIZE] = {0};
    tableInfoFile.read(tableInfoBlob, MAX_PAGE_SIZE);
    tableInfoFile.close();
    uint64_t offset = 0;
    // Table name
    this->name = std::string(tableInfoBlob + offset);
    offset += this->name.size() + 1;
    // Column count
    size_t column_count = 0;
    memcpy(&column_count, tableInfoBlob + offset, sizeof(size_t));
    offset += sizeof(size_t);
    // Column names
    for (int i = 0; i < column_count; ++i) {
        std::string column_name = std::string(tableInfoBlob + offset);
        this->column_names.push_back(column_name);
        offset += column_name.size() + 1;
    }

    // Column types
    for (int i = 0; i < column_count; ++i) {
        char column_type = 0;
        memcpy(&column_type, tableInfoBlob + offset, sizeof(char));
        this->column_types.push_back((Type)column_type);
        offset += sizeof(char);
    }

    // Next row index
    memcpy(&this->next_row_idx, tableInfoBlob + offset, sizeof(int64_t));
    offset += sizeof(int64_t);

    // Primary key column
    this->primary_key_column = std::string(tableInfoBlob + offset);
    offset += this->primary_key_column.size() + 1;

    // Is unique
    for (int i = 0; i < column_count; ++i) {
        bool temp = false;
        memcpy(&temp, tableInfoBlob + offset, sizeof(bool));
        this->is_unique.push_back(temp);
        offset += sizeof(bool);
    }

    // Is not null
    for (int i = 0; i < column_count; ++i) {
        bool temp = false;
        memcpy(&temp, tableInfoBlob + offset, sizeof(bool));
        this->is_not_null.push_back(temp);
        offset += sizeof(bool);
    }

    // Default value
    for (int i = 0; i < column_count; ++i) {
        std::string default_value = std::string(tableInfoBlob + offset);
        this->default_values.push_back(default_value);
        offset += default_value.size() + 1;
    }

    // Rows size
    size_t rows_size = 0;
    memcpy(&rows_size, tableInfoBlob + offset, sizeof(size_t));
    offset += sizeof(size_t);
    // this->rows.resize(rows_size);

    std::cout << "Loading indexes " << column_count << std::endl;

    std::vector<std::string> indexes_name;
    for (int i = 0; i < column_count; ++i) {
        std::string index_name = std::string(tableInfoBlob + offset);
        std::cout << "Loading index " << index_name << std::endl;
        if (index_name == "") {
            offset += index_name.size() + 1;
            break;
        }
        indexes_name.push_back(index_name);
        offset += index_name.size() + 1;
    }

    // Load rows
    // Loop through rows folder
    // size_t idx = 0;
    // std::vector<std::filesystem::path> files_in_directory;
    // std::copy(
    //     std::filesystem::directory_iterator(dbFolderPath.string() + "/rows"),
    //     std::filesystem::directory_iterator(),
    //     std::back_inserter(files_in_directory));
    // std::sort(
    //     files_in_directory.begin(), files_in_directory.end(),
    //     [&](const std::filesystem::path &a, const std::filesystem::path &b) {
    //         return std::stoul(a.filename().string()) <
    //                std::stoul(b.filename().string());
    //     });
    // for (auto &p : files_in_directory) {
    //     std::string row_name = p.filename().string();
    //     std::ifstream file(p.string());
    //     this->rows[idx++] = std::make_shared<Row>(file);
    //     file.close();
    // }

    // Load indexes
    // for (int i = 0; i < indexes_name.size(); ++i) {
    //     std::cout << "Loading index " << indexes_name[i] << std::endl;
    //     this->indexes[Index(this->name, indexes_name[i])] = BTree::load(
    //         dbFolderPath.string(),
    //         Index(this->name, indexes_name[i]).toString(), this->rows);
    // }
}