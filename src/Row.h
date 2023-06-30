#ifndef ROW_H
#define ROW_H

#include <string.h>

#include <fstream>
#include <memory>
#include <vector>

#include "Type.h"

struct Row {
    std::shared_ptr<void> data = nullptr;
    std::vector<char> is_set;
    std::vector<uint64_t> sizes;
    uint64_t headerSize = 0;
    uint64_t totalSize = 0;
    Row(std::ifstream &file) { load(file); }
    Row(void *data, std::vector<uint64_t> sizes, std::vector<char> is_set) {
        this->sizes = sizes;
        this->data = std::shared_ptr<void>(data, free);
        this->is_set = is_set;
    }

    ~Row() {}

    size_t computeHeaderSize() {
        headerSize = 0;
        // column size
        headerSize += sizes.size() * sizeof(uint64_t);
        // column type
        headerSize += sizes.size() * sizeof(char);
        return headerSize;
    }

    size_t computeHeaderSize(const std::vector<int> &requestedIndexes) {
        headerSize = 0;
        // column size
        headerSize += requestedIndexes.size() * sizeof(uint64_t);
        // column type
        headerSize += requestedIndexes.size() * sizeof(char);
        return headerSize;
    }

    size_t computeTotalSize() {
        totalSize = 0;
        totalSize += computeHeaderSize();
        for (int i = 0; i < sizes.size(); ++i) totalSize += sizes[i];
        return totalSize;
    }

    size_t computeTotalSize(const std::vector<int> &requestedIndexes) {
        totalSize = 0;
        totalSize += computeHeaderSize(requestedIndexes);
        for (int i = 0; i < requestedIndexes.size(); ++i)
            totalSize += this->sizes[requestedIndexes[i]];
        return totalSize;
    }

    void *blob() {
        computeHeaderSize();
        computeTotalSize();
        void *blob = malloc(totalSize);
        memcpy((char *)blob, data.get(), totalSize - headerSize);
        return blob;
    }

    void *blob(const std::vector<int> &requestedIndexes) {
        computeHeaderSize(requestedIndexes);
        computeTotalSize(requestedIndexes);
        void *blob = malloc(totalSize);
        uint64_t offset = 0;
        size_t column_count = requestedIndexes.size();
        for (int i = 0; i < requestedIndexes.size(); ++i) {
            memcpy((char *)blob + offset, &this->sizes[requestedIndexes[i]],
                   sizeof(uint64_t));
            offset += sizeof(uint64_t);
        }
        for (int i = 0; i < requestedIndexes.size(); ++i) {
            memcpy((char *)blob + offset, &this->is_set[requestedIndexes[i]],
                   sizeof(char));
            offset += sizeof(char);
        }
        for (int i = 0; i < requestedIndexes.size(); ++i) {
            if (is_set[requestedIndexes[i]]) {
                size_t offsetInData = 0;
                for (int j = 0; j < requestedIndexes[i]; ++j)
                    offsetInData += this->sizes[j];
                memcpy((char *)blob + offset,
                       (char *)this->data.get() + offsetInData,
                       this->sizes[requestedIndexes[i]]);
                offset += this->sizes[requestedIndexes[i]];
            }
        }
        return blob;
    }

    void *getKey(size_t index, Type type) {
        void *key = malloc(this->sizes[index] + (type == Type::CHAR ||
                                                 type == Type::VARCHAR ||
                                                 type == Type::TEXT));
        size_t offset = 0;
        for (int i = 0; i < index; ++i) offset += this->sizes[i];
        memcpy(key, (char *)this->data.get() + offset, this->sizes[index]);
        if (type == Type::CHAR || type == Type::VARCHAR || type == Type::TEXT)
            ((char *)key)[this->sizes[index]] = '\0';
        return key;
    }

    void save(std::ofstream &file) {
        computeTotalSize();
        file.write((char *)&headerSize, sizeof(uint64_t));
        file.write((char *)&totalSize, sizeof(uint64_t));
        uint64_t size = sizes.size();
        file.write((char *)&size, sizeof(uint64_t));
        // column size
        for (int i = 0; i < sizes.size(); ++i)
            file.write((char *)&sizes[i], sizeof(uint64_t));
        // is_set
        for (int i = 0; i < is_set.size(); ++i)
            file.write((char *)&is_set[i], sizeof(char));
        void *b = blob();
        file.write((char *)b, totalSize - headerSize);
        free(b);
    }

    void load(std::ifstream &file) {
        file.read((char *)&headerSize, sizeof(uint64_t));
        file.read((char *)&totalSize, sizeof(uint64_t));
        uint64_t size;
        file.read((char *)&size, sizeof(uint64_t));
        sizes.resize(size);
        is_set.resize(size);
        // column size
        for (int i = 0; i < sizes.size(); ++i)
            file.read((char *)&sizes[i], sizeof(uint64_t));
        // is_set
        for (int i = 0; i < is_set.size(); ++i)
            file.read((char *)&is_set[i], sizeof(char));
        void *b = malloc(totalSize - headerSize);
        file.read((char *)b, totalSize - headerSize);
        data = std::shared_ptr<void>(b, free);
    }
};

#endif