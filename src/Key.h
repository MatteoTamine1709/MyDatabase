#ifndef KEY_H
#define KEY_H

#include <stdlib.h>
#include <string.h>

#include <memory>
#include <string>

#include "utils.h"

struct Key {
    std::shared_ptr<void> data = nullptr;
    size_t keySize = 0;
    Key() = default;
    Key(void *data, size_t keySize) {
        this->data = std::shared_ptr<void>(data, free);
        this->keySize = keySize;
    }
    ~Key() {}
    bool operator<(const Key &other) const {
        if (this->data == nullptr) return false;
        if (other.data == nullptr) return true;

        return utils::reverse_memcmp(this->data.get(), other.data.get(),
                                     this->keySize) < 0;
    }
    bool operator>(const Key &other) const {
        if (this->data == nullptr) return true;
        if (other.data == nullptr) return false;
        return utils::reverse_memcmp(this->data.get(), other.data.get(),
                                     this->keySize) > 0;
    }
    bool operator==(const Key &other) const {
        if (this->data == nullptr) return false;
        if (other.data == nullptr) return false;
        return utils::reverse_memcmp(this->data.get(), other.data.get(),
                                     this->keySize) == 0;
    }
    bool operator!=(const Key &other) const {
        if (this->data == nullptr) return true;
        if (other.data == nullptr) return true;
        return utils::reverse_memcmp(this->data.get(), other.data.get(),
                                     this->keySize) != 0;
    }
    bool operator<=(const Key &other) const {
        if (this->data == nullptr) return false;
        if (other.data == nullptr) return true;
        return utils::reverse_memcmp(this->data.get(), other.data.get(),
                                     this->keySize) <= 0;
    }
    bool operator>=(const Key &other) const {
        if (this->data == nullptr) return true;
        if (other.data == nullptr) return false;
        return utils::reverse_memcmp(this->data.get(), other.data.get(),
                                     this->keySize) >= 0;
    }
    std::string toString() const {
        uint32_t v = *(uint32_t *)this->data.get();
        return std::to_string(v);
    }
};

#endif