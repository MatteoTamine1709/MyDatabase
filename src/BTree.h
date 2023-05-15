#ifndef BTREE_H
#define BTREE_H

#include <array>
#include <iostream>
#include <memory>
#include <type_traits>
#include <utility>

#include "Condition.h"
#include "Key.h"
#include "Row.h"
#define BTREE_DEGREE 3

class TreeNode {
   public:
    std::array<TreeNode *, 2 *BTREE_DEGREE> C = {nullptr};
    int n = 0;
    bool leaf = false;
    Key keys[2 * BTREE_DEGREE - 1] = {};
    std::unique_ptr<Row> data[2 * BTREE_DEGREE - 1] = {nullptr};
    TreeNode(bool leaf, Row &&data);
    TreeNode(bool leaf) : leaf(leaf){};
    ~TreeNode() {
        for (int i = 0; i <= n; ++i) delete C[i];
    }

    void remove(const Key &k);
    void removeLeaf(int idx);
    void removeNonLeaf(int idx);
    void fill(int idx);
    std::pair<Key, std::unique_ptr<Row> &> getPred(int idx);
    std::pair<Key, std::unique_ptr<Row> &> getSucc(int idx);
    void merge(int idx);
    void borrowFromPrev(int idx);
    void borrowFromNext(int idx);

    void insertNonFull(const Key &k, Row data);
    void splitChild(int i, TreeNode *y);
    void traverse();
    void prettyPrint(int level);

    std::pair<Key *, Row *> search(Key k);
    void searchRange(const Key &start, const Key &end,
                     std::pair<bool, bool> isInclusive,
                     std::vector<std::pair<Key *, Row *>> &result);

    void save(std::string indexPathFolder, int &index);
    static TreeNode *load(std::string filepath, int &index);
};

class BTree {
    static_assert(!std::is_pointer<Row>::value,
                  "The Row must not be a pointer type");
    static_assert(
        std::is_same<decltype(std::declval<Key>() > std::declval<Key>()),
                     bool>::value,
        "The Key must implement > operator");
    static_assert(
        std::is_same<decltype(std::declval<Key>() < std::declval<Key>()),
                     bool>::value,
        "The Key must implement < operator");
    static_assert(
        std::is_same<decltype(std::declval<Key>() == std::declval<Key>()),
                     bool>::value,
        "The Key must implement == operator");
    TreeNode *root = nullptr;

   public:
    BTree() = default;
    ~BTree() {
        if (root != nullptr) delete root;
    }

    void traverse() {
        if (root != nullptr) root->traverse();
    }

    void prettyPrint() {
        if (root == nullptr)
            std::cout << "The tree is empty\n";
        else
            root->prettyPrint(0);
    }

    std::pair<Key *, Row *> search(const Key &k) {
        return (root == nullptr) ? std::pair<Key *, Row *>(nullptr, nullptr)
                                 : root->search(k);
    }

    std::vector<std::pair<Key *, Row *>> searchRange(
        const std::vector<Condition> &conditions) {
        std::pair<bool, bool> isInclusive = {true, true};
        std::vector<std::pair<Key *, Row *>> result;
        for (auto condition : conditions) {
            auto ranges = condition.generateRanges();
            if (condition.op == "<") isInclusive.second = false;
            if (condition.op == ">") isInclusive.first = false;
            if (condition.op == "<=") isInclusive = {true, true};
            if (condition.op == ">=") isInclusive = {true, true};
            if (condition.op == "=") isInclusive = {true, true};
            if (condition.op == "!=") isInclusive = {false, false};
            for (auto range : ranges) {
                auto temp = searchRange(range.first, range.second, isInclusive);
                result.insert(result.end(), temp.begin(), temp.end());
            }
        }

        if (conditions.size() == 0)
            return searchRange(Key(), Key(), isInclusive);
        return result;
    }
    std::vector<std::pair<Key *, Row *>> searchRange(
        const Key &start, const Key &end, std::pair<bool, bool> isInclusive) {
        std::vector<std::pair<Key *, Row *>> result;
        if (root == nullptr) return result;

        root->searchRange(start, end, isInclusive, result);
        return result;
    }

    void insert(const Key &k, Row &&data);

    void remove(const Key &k);

    void save(std::string indexPathFolder);
    static BTree *load(std::string filepath);
};

#endif