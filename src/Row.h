#ifndef ROW_H
#define ROW_H

#include <string.h>

#include <memory>
#include <vector>

#include "Type.h"

struct Row {
    std::shared_ptr<void> data;
    std::vector<char> is_set;
    std::vector<uint64_t> sizes;
    uint64_t headerSize = 0;
    uint64_t rowSize = 0;
    uint64_t totalSize = 0;
    Row(void *data, std::vector<uint64_t> sizes, std::vector<char> is_set) {
        this->sizes = sizes;
        this->data = std::shared_ptr<void>(data, free);
        this->is_set = is_set;
    }

    ~Row() {}

    size_t computeHeaderSize(const std::vector<int> &requestedIndexes) {
        headerSize = 0;
        // column size
        headerSize += requestedIndexes.size() * sizeof(uint64_t);
        // column type
        headerSize += requestedIndexes.size() * sizeof(char);
        return headerSize;
    }
    size_t computeTotalSize(const std::vector<int> &requestedIndexes) {
        totalSize = 0;
        totalSize += computeHeaderSize(requestedIndexes);
        for (int i = 0; i < requestedIndexes.size(); ++i)
            totalSize += this->sizes[requestedIndexes[i]];
        return totalSize;
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
};

#endif