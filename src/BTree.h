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
    int64_t data[2 * BTREE_DEGREE - 1] = {-1};
    TreeNode(bool leaf, int64_t data);
    TreeNode(bool leaf) : leaf(leaf){};
    ~TreeNode() {
        for (int i = 0; i <= n; ++i) delete C[i];
    }

    void remove(const Key &k);
    void removeLeaf(int idx);
    void removeNonLeaf(int idx);
    void fill(int idx);
    std::pair<Key, int64_t> getPred(int idx);
    std::pair<Key, int64_t> getSucc(int idx);
    void merge(int idx);
    void borrowFromPrev(int idx);
    void borrowFromNext(int idx);

    void insertNonFull(const Key &k, int64_t data);
    void splitChild(int i, TreeNode *y);
    void traverse();
    void prettyPrint(int level);

    std::pair<Key *, int64_t> search(Key k);
    void searchRange(const Key &start, const Key &end,
                     std::pair<bool, bool> isInclusive,
                     std::vector<std::pair<Key *, int64_t>> &result);

    void save(std::string indexPathFolder, int &index);
    static TreeNode *load(std::string filepath, std::string rowFolderpath,
                          int &index, std::vector<std::shared_ptr<Row>> &rows);
};

class BTree {
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

    std::pair<Key *, int64_t> search(const Key &k) {
        return (root == nullptr) ? std::pair<Key *, int64_t>(nullptr, -1)
                                 : root->search(k);
    }

    std::vector<std::pair<Key *, int64_t>> searchRange(
        const std::vector<Condition> &conditions) {
        std::pair<bool, bool> isInclusive = {true, true};
        std::vector<std::pair<Key *, int64_t>> result;
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
    std::vector<std::pair<Key *, int64_t>> searchRange(
        const Key &start, const Key &end, std::pair<bool, bool> isInclusive) {
        std::vector<std::pair<Key *, int64_t>> result;
        if (root == nullptr) return result;

        root->searchRange(start, end, isInclusive, result);
        return result;
    }

    void insert(const Key &k, int64_t data);

    void remove(const Key &k);

    void save(std::string indexPathFolder);
    static BTree *load(std::string filepath, std::string indexName,
                       std::vector<std::shared_ptr<Row>> &data);
};

#endif