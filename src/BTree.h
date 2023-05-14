#ifndef BTREE_H
#define BTREE_H

#include <array>
#include <iostream>
#include <memory>
#include <type_traits>
#define BTREE_DEGREE 3

template <typename KEY, typename DATA>
class TreeNode {
   public:
    std::array<TreeNode *, 2 *BTREE_DEGREE> C = {nullptr};
    int n = 0;
    bool leaf = false;
    KEY keys[2 * BTREE_DEGREE - 1] = {KEY()};
    std::unique_ptr<DATA> data[2 * BTREE_DEGREE - 1] = {nullptr};
    TreeNode(bool leaf, DATA data);
    TreeNode(bool leaf) : leaf(leaf){};

    void remove(const KEY &k);
    void removeLeaf(int idx);
    void removeNonLeaf(int idx);
    void fill(int idx);
    std::pair<KEY, std::unique_ptr<DATA> &> getPred(int idx);
    std::pair<KEY, std::unique_ptr<DATA> &> getSucc(int idx);
    void merge(int idx);
    void borrowFromPrev(int idx);
    void borrowFromNext(int idx);

    void insertNonFull(const KEY &k, DATA data);
    void splitChild(int i, TreeNode<KEY, DATA> *y);
    void traverse();
    void prettyPrint(int level);

    std::pair<KEY *, DATA *> search(KEY k);
};

template <typename KEY, typename DATA>
class BTree {
    static_assert(!std::is_pointer<DATA>::value,
                  "The DATA must not be a pointer type");
    static_assert(
        std::is_same<decltype(std::declval<KEY>() > std::declval<KEY>()),
                     bool>::value,
        "The KEY must implement > operator");
    static_assert(
        std::is_same<decltype(std::declval<KEY>() < std::declval<KEY>()),
                     bool>::value,
        "The KEY must implement < operator");
    static_assert(
        std::is_same<decltype(std::declval<KEY>() == std::declval<KEY>()),
                     bool>::value,
        "The KEY must implement == operator");
    TreeNode<KEY, DATA> *root = nullptr;

   public:
    BTree() = default;

    template <typename K = KEY, typename D = void,
              typename std::enable_if<std::is_same<
                  decltype(std::declval<std::ostream &>() << std::declval<K>()),
                  std::ostream &>::value>::type * = nullptr>
    void traverse() {
        if (root != nullptr) root->traverse();
    }

    template <typename K = KEY, typename D = void,
              typename std::enable_if<std::is_same<
                  decltype(std::declval<std::ostream &>() << std::declval<K>()),
                  std::ostream &>::value>::type * = nullptr>
    void prettyPrint() {
        if (root == nullptr)
            std::cout << "The tree is empty\n";
        else
            root->prettyPrint(0);
    }

    std::pair<KEY *, DATA *> search(const KEY &k) {
        return (root == nullptr) ? std::pair<KEY *, DATA *>(nullptr, nullptr)
                                 : root->search(k);
    }

    void insert(const KEY &k, DATA data);

    void remove(const KEY &k);
};

template <typename KEY, typename DATA>
TreeNode<KEY, DATA>::TreeNode(bool leaf, DATA data) : leaf(leaf), n(0) {
    this->data[0] = std::make_unique<DATA>(data);
}
template <typename KEY, typename DATA>
void TreeNode<KEY, DATA>::traverse() {
    int i;
    for (i = 0; i < n; i++) {
        if (leaf == false) C[i]->traverse();
        std::cout << " " << keys[i];
    }

    if (leaf == false) C[i]->traverse();
}

template <typename KEY, typename DATA>
void TreeNode<KEY, DATA>::prettyPrint(int level) {
    if (leaf) {
        // Color green
        std::cout << "\033[1;32m";
        for (int i = 0; i < level; ++i) std::cout << "\t";
        for (int i = 0; i < n; ++i) std::cout << keys[i] << " ";
        std::cout << std::endl;
        // Change color to default
        std::cout << "\033[0m";
    } else {
        int idx = 0;
        for (int j = 0; j < n; ++j) {
            for (; idx < n && C[idx]->keys[0] < keys[j]; ++idx)
                C[idx]->prettyPrint(level + 1);
            // Color Red
            if (level % 2) std::cout << "\033[1;31m";
            for (int i = 0; i < level; ++i) std::cout << "\t";
            std::cout << keys[j] << std::endl;
            std::cout << "\033[0m";
        }
        C[n]->prettyPrint(level + 1);
    }
}

template <typename KEY, typename DATA>
std::pair<KEY *, DATA *> TreeNode<KEY, DATA>::search(KEY k) {
    int i = 0;
    while (i < n && k > keys[i]) i++;

    if (keys[i] == k && i < n)
        return std::pair<KEY *, DATA *>(&keys[i], data[i].get());

    if (leaf == true) return std::pair<KEY *, DATA *>(nullptr, nullptr);

    return C[i]->search(k);
}

template <typename KEY, typename DATA>
void BTree<KEY, DATA>::insert(const KEY &k, DATA data) {
    if (root == nullptr) {
        root = new TreeNode<KEY, DATA>(true, data);
        root->keys[0] = k;
        root->n = 1;
    } else {
        if (root->n == 2 * BTREE_DEGREE - 1) {
            TreeNode<KEY, DATA> *s = new TreeNode<KEY, DATA>(false, data);

            s->C[0] = root;

            s->splitChild(0, root);

            int i = 0;
            if (s->keys[0] < k) i++;
            s->C[i]->insertNonFull(k, data);

            root = s;
        } else
            root->insertNonFull(k, data);
    }
}

template <typename KEY, typename DATA>
void BTree<KEY, DATA>::remove(const KEY &k) {
    if (!root) {
        // std::cout << "The tree is empty\n";
        return;
    }

    root->remove(k);

    if (root->n == 0) {
        TreeNode<KEY, DATA> *tmp = root;
        if (root->leaf)
            root = nullptr;
        else
            root = root->C[0];

        delete tmp;
    }
}

template <typename KEY, typename DATA>
void TreeNode<KEY, DATA>::remove(const KEY &k) {
    int idx = 0;
    while (idx < n && keys[idx] < k) ++idx;

    if (idx < n && keys[idx] == k) {
        if (leaf)
            removeLeaf(idx);
        else
            removeNonLeaf(idx);
    } else {
        if (leaf) {
            // std::cout << "The key " << k << " is does not exist in the
            // tree\n";
            return;
        }

        bool flag = idx == n;

        if (C[idx]->n < BTREE_DEGREE) fill(idx);

        if (flag && idx > n)
            C[idx - 1]->remove(k);
        else
            C[idx]->remove(k);
    }
    return;
}

template <typename KEY, typename DATA>
void TreeNode<KEY, DATA>::removeLeaf(int idx) {
    for (int i = idx + 1; i < n; ++i) {
        keys[i - 1] = keys[i];
        data[i - 1] = std::move(data[i]);
    }
    n--;
}

template <typename KEY, typename DATA>
void TreeNode<KEY, DATA>::removeNonLeaf(int idx) {
    KEY k = keys[idx];

    std::cout << "N: " << C[idx]->n << std::endl;

    if (C[idx]->n >= BTREE_DEGREE) {
        auto pred = getPred(idx);
        std::cout << "Pred: " << pred.first << ", " << pred.second.get()
                  << std::endl;
        keys[idx] = pred.first;
        data[idx] = std::move(pred.second);
        C[idx]->remove(pred.first);
    } else if (C[idx + 1]->n >= BTREE_DEGREE) {
        auto succ = getSucc(idx);
        keys[idx] = succ.first;
        data[idx] = std::move(succ.second);
        C[idx + 1]->remove(succ.first);
    } else {
        merge(idx);
        C[idx]->remove(k);
    }
    return;
}

template <typename KEY, typename DATA>
void TreeNode<KEY, DATA>::fill(int idx) {
    if (idx != 0 && C[idx - 1]->n >= BTREE_DEGREE)
        borrowFromPrev(idx);

    else if (idx != n && C[idx + 1]->n >= BTREE_DEGREE)
        borrowFromNext(idx);

    else {
        if (idx != n)
            merge(idx);
        else
            merge(idx - 1);
    }
    return;
}

template <typename KEY, typename DATA>
std::pair<KEY, std::unique_ptr<DATA> &> TreeNode<KEY, DATA>::getPred(int idx) {
    TreeNode<KEY, DATA> *cur = C[idx];
    while (!cur->leaf) cur = cur->C[cur->n];

    return std::pair<KEY, std::unique_ptr<DATA> &>(cur->keys[cur->n - 1],
                                                   cur->data[cur->n - 1]);
}

template <typename KEY, typename DATA>
std::pair<KEY, std::unique_ptr<DATA> &> TreeNode<KEY, DATA>::getSucc(int idx) {
    TreeNode<KEY, DATA> *cur = C[idx + 1];
    while (!cur->leaf) cur = cur->C[0];

    return std::pair<KEY, std::unique_ptr<DATA> &>(cur->keys[0], cur->data[0]);
}

template <typename KEY, typename DATA>
void TreeNode<KEY, DATA>::borrowFromPrev(int idx) {
    TreeNode<KEY, DATA> *child = C[idx];
    TreeNode<KEY, DATA> *sibling = C[idx - 1];

    for (int i = child->n - 1; i >= 0; --i) {
        child->keys[i + 1] = child->keys[i];
        child->data[i + 1] = std::move(child->data[i]);
    }

    if (!child->leaf) {
        for (int i = child->n; i >= 0; --i) child->C[i + 1] = child->C[i];
    }

    child->keys[0] = keys[idx - 1];
    child->data[0] = std::move(data[idx - 1]);

    if (!child->leaf) child->C[0] = sibling->C[sibling->n];

    keys[idx - 1] = sibling->keys[sibling->n - 1];
    data[idx - 1] = std::move(sibling->data[sibling->n - 1]);

    child->n += 1;
    sibling->n -= 1;

    return;
}

template <typename KEY, typename DATA>
void TreeNode<KEY, DATA>::borrowFromNext(int idx) {
    TreeNode<KEY, DATA> *child = C[idx];
    TreeNode<KEY, DATA> *sibling = C[idx + 1];

    child->keys[(child->n)] = keys[idx];
    child->data[(child->n)] = std::move(data[idx]);

    if (!(child->leaf)) child->C[(child->n) + 1] = sibling->C[0];

    keys[idx] = sibling->keys[0];
    data[idx] = std::move(sibling->data[0]);

    for (int i = 1; i < sibling->n; ++i) {
        sibling->keys[i - 1] = sibling->keys[i];
        sibling->data[i - 1] = std::move(sibling->data[i]);
    }

    if (!sibling->leaf) {
        for (int i = 1; i <= sibling->n; ++i) sibling->C[i - 1] = sibling->C[i];
    }

    child->n += 1;
    sibling->n -= 1;

    return;
}

template <typename KEY, typename DATA>
void TreeNode<KEY, DATA>::merge(int idx) {
    TreeNode<KEY, DATA> *child = C[idx];
    TreeNode<KEY, DATA> *sibling = C[idx + 1];

    child->keys[BTREE_DEGREE - 1] = keys[idx];
    child->data[BTREE_DEGREE - 1] = std::move(data[idx]);

    for (int i = 0; i < sibling->n; ++i) {
        child->keys[i + BTREE_DEGREE] = sibling->keys[i];
        child->data[i + BTREE_DEGREE] = std::move(sibling->data[i]);
    }

    if (!child->leaf) {
        for (int i = 0; i <= sibling->n; ++i)
            child->C[i + BTREE_DEGREE] = sibling->C[i];
    }

    for (int i = idx + 1; i < n; ++i) {
        keys[i - 1] = keys[i];
        data[i - 1] = std::move(data[i]);
    }

    for (int i = idx + 2; i <= n; ++i) C[i - 1] = C[i];

    child->n += sibling->n + 1;
    n--;

    delete (sibling);
    return;
}

template <typename KEY, typename DATA>
void TreeNode<KEY, DATA>::insertNonFull(const KEY &k, DATA data) {
    int i = n - 1;

    if (leaf == true) {
        while (i >= 0 && keys[i] > k) {
            keys[i + 1] = keys[i];
            this->data[i + 1] = std::move(this->data[i]);
            i--;
        }

        keys[i + 1] = k;
        this->data[i + 1] = std::make_unique<DATA>(data);
        n = n + 1;
    } else {
        while (i >= 0 && keys[i] > k) i--;

        if (C[i + 1]->n == 2 * BTREE_DEGREE - 1) {
            splitChild(i + 1, C[i + 1]);

            if (keys[i + 1] < k) i++;
        }
        C[i + 1]->insertNonFull(k, data);
    }
}

template <typename KEY, typename DATA>
void TreeNode<KEY, DATA>::splitChild(int i, TreeNode<KEY, DATA> *y) {
    TreeNode *z = new TreeNode(y->leaf);
    z->n = BTREE_DEGREE - 1;

    for (int j = 0; j < BTREE_DEGREE - 1; j++) {
        z->keys[j] = y->keys[j + BTREE_DEGREE];
        z->data[j] = std::move(y->data[j + BTREE_DEGREE]);
    }

    if (y->leaf == false) {
        for (int j = 0; j < BTREE_DEGREE; j++) z->C[j] = y->C[j + BTREE_DEGREE];
    }

    y->n = BTREE_DEGREE - 1;
    for (int j = n; j >= i + 1; j--) C[j + 1] = C[j];

    C[i + 1] = z;

    for (int j = n - 1; j >= i; j--) {
        keys[j + 1] = keys[j];
        data[j + 1] = std::move(data[j]);
    }

    keys[i] = y->keys[BTREE_DEGREE - 1];
    data[i] = std::move(y->data[BTREE_DEGREE - 1]);
    n = n + 1;
}

#endif