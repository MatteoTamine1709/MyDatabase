#ifndef KEY_H
#define KEY_H

#include <stdlib.h>
#include <string.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "utils.h"

struct Key {
    std::shared_ptr<void> data = nullptr;
    Type keyType = Type::INVALID;
    Key() = default;
    Key(void *data, Type keyType) {
        this->data = std::shared_ptr<void>(data, free);
        this->keyType = keyType;
    }
    ~Key() {}
    bool operator<(const Key &other) const {
        if (this->data == nullptr) return false;
        if (other.data == nullptr) return true;

        if (this->keyType == Type::CHAR || this->keyType == Type::VARCHAR ||
            this->keyType == Type::TEXT)
            return strcmp((char *)this->data.get(), (char *)other.data.get()) <
                   0;

        return utils::reverse_memcmp(this->data.get(), other.data.get(),
                                     type_max_size[this->keyType]) < 0;
    }
    bool operator>(const Key &other) const {
        if (this->data == nullptr) return true;
        if (other.data == nullptr) return false;

        if (this->keyType == Type::CHAR || this->keyType == Type::VARCHAR ||
            this->keyType == Type::TEXT)
            return strcmp((char *)this->data.get(), (char *)other.data.get()) >
                   0;

        return utils::reverse_memcmp(this->data.get(), other.data.get(),
                                     type_max_size[this->keyType]) > 0;
    }
    bool operator==(const Key &other) const {
        if (this->data == nullptr) return false;
        if (other.data == nullptr) return false;

        if (this->keyType == Type::CHAR || this->keyType == Type::VARCHAR ||
            this->keyType == Type::TEXT)
            return strcmp((char *)this->data.get(), (char *)other.data.get()) ==
                   0;

        return utils::reverse_memcmp(this->data.get(), other.data.get(),
                                     type_max_size[this->keyType]) == 0;
    }
    bool operator!=(const Key &other) const {
        if (this->data == nullptr) return true;
        if (other.data == nullptr) return true;

        if (this->keyType == Type::CHAR || this->keyType == Type::VARCHAR ||
            this->keyType == Type::TEXT)
            return strcmp((char *)this->data.get(), (char *)other.data.get()) !=
                   0;

        return utils::reverse_memcmp(this->data.get(), other.data.get(),
                                     type_max_size[this->keyType]) != 0;
    }
    bool operator<=(const Key &other) const {
        if (this->data == nullptr) return false;
        if (other.data == nullptr) return true;

        if (this->keyType == Type::CHAR || this->keyType == Type::VARCHAR ||
            this->keyType == Type::TEXT)
            return strcmp((char *)this->data.get(), (char *)other.data.get()) <=
                   0;

        return utils::reverse_memcmp(this->data.get(), other.data.get(),
                                     type_max_size[this->keyType]) <= 0;
    }
    bool operator>=(const Key &other) const {
        if (this->data == nullptr) return true;
        if (other.data == nullptr) return false;

        if (this->keyType == Type::CHAR || this->keyType == Type::VARCHAR ||
            this->keyType == Type::TEXT)
            return strcmp((char *)this->data.get(), (char *)other.data.get()) >=
                   0;

        return utils::reverse_memcmp(this->data.get(), other.data.get(),
                                     type_max_size[this->keyType]) >= 0;
    }
    std::string toString() const {
        if (this->data == nullptr) return "NULL";
        if (this->keyType == Type::CHAR || this->keyType == Type::VARCHAR ||
            this->keyType == Type::TEXT)
            return std::string((char *)this->data.get());
        if (this->keyType == Type::INTEGER)
            return std::to_string(*(int *)this->data.get());
        if (this->keyType == Type::FLOAT)
            return std::to_string(*(float *)this->data.get());
        if (this->keyType == Type::DECIMAL)
            return std::to_string(*(double *)this->data.get());
        if (this->keyType == Type::BOOLEAN)
            return (*(bool *)this->data.get()) ? "true" : "false";
        return std::to_string(*(uint32_t *)this->data.get());
    }

    void save(std::ofstream &file) {
        file.write((char *)&keyType, sizeof(size_t));
        file.write((char *)data.get(), keyType);
    }

    void load(std::ifstream &file) {
        file.read((char *)&keyType, sizeof(size_t));
        void *data = malloc(keyType);
        file.read((char *)data, keyType);
        this->data = std::shared_ptr<void>(data, free);
    }
};

#endif