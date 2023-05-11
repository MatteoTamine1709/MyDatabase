#ifndef BTREE_H
#define BTREE_H

#include <array>
#include <iostream>
#include <type_traits>
#define BTREE_DEGREE 3

template <typename KEY, typename DATA>
class TreeNode
{
public:
    std::array<TreeNode *, 2 *BTREE_DEGREE> C = {nullptr};
    int n = 0;
    bool leaf = false;
    KEY keys[2 * BTREE_DEGREE - 1] = {KEY()};
    DATA data = nullptr;
    TreeNode(bool leaf, DATA data = nullptr);

    void insertNonFull(KEY k, DATA data);
    void splitChild(int i, TreeNode<KEY, DATA> *y);
    void traverse();

    TreeNode *search(KEY k);
};

template <typename KEY, typename DATA>
class BTree
{

    static_assert(std::is_same<decltype(std::declval<KEY>() > std::declval<KEY>()), bool>::value, "The KEY must implement > operator");
    static_assert(std::is_same<decltype(std::declval<KEY>() < std::declval<KEY>()), bool>::value, "The KEY must implement < operator");
    static_assert(std::is_same<decltype(std::declval<KEY>() == std::declval<KEY>()), bool>::value, "The KEY must implement == operator");
    // static_assert(std::is_pointer<KEY>::value, "The DATA must be a pointer type");
    static_assert(std::is_pointer<DATA>::value, "The DATA must be a pointer type");
    TreeNode<KEY, DATA> *root = nullptr;

public:
    BTree() = default;

    template <typename K = KEY, typename D = void,
              typename std::enable_if<
                  std::is_same<decltype(std::declval<std::ostream &>() << std::declval<K>()),
                               std::ostream &>::value>::type * = nullptr>
    void traverse()
    {
        if (root != nullptr)
            root->traverse();
    }

    TreeNode<KEY, DATA> *search(KEY k)
    {
        return (root == nullptr) ? nullptr : root->search(k);
    }

    void insert(KEY k, DATA data);
};

template <typename KEY, typename DATA>
TreeNode<KEY, DATA>::TreeNode(bool leaf, DATA data)
    : leaf(leaf), n(0), data(data) {}
template <typename KEY, typename DATA>
void TreeNode<KEY, DATA>::traverse()
{
    int i;
    for (i = 0; i < n; i++)
    {
        if (leaf == false)
            C[i]->traverse();
        std::cout << " " << keys[i];
    }

    if (leaf == false)
        C[i]->traverse();
}
template <typename KEY, typename DATA>
TreeNode<KEY, DATA> *TreeNode<KEY, DATA>::search(KEY k)
{
    int i = 0;
    while (i < n && k > keys[i])
        i++;

    if (keys[i] == k)
        return this;

    if (leaf == true)
        return nullptr;

    return C[i]->search(k);
}

template <typename KEY, typename DATA>
void BTree<KEY, DATA>::insert(KEY k, DATA data)
{
    if (root == nullptr)
    {
        root = new TreeNode<KEY, DATA>(true, data);
        root->keys[0] = k;
        root->n = 1;
    }
    else
    {
        if (root->n == 2 * BTREE_DEGREE - 1)
        {
            TreeNode<KEY, DATA> *s = new TreeNode<KEY, DATA>(false, data);

            s->C[0] = root;

            s->splitChild(0, root);

            int i = 0;
            if (s->keys[0] < k)
                i++;
            s->C[i]->insertNonFull(k, data);

            root = s;
        }
        else
            root->insertNonFull(k, data);
    }
}

template <typename KEY, typename DATA>
void TreeNode<KEY, DATA>::insertNonFull(KEY k, DATA data)
{
    int i = n - 1;

    if (leaf == true)
    {
        while (i >= 0 && keys[i] > k)
        {
            keys[i + 1] = keys[i];
            i--;
        }

        keys[i + 1] = k;
        n = n + 1;
    }
    else
    {
        while (i >= 0 && keys[i] > k)
            i--;

        if (C[i + 1]->n == 2 * BTREE_DEGREE - 1)
        {
            splitChild(i + 1, C[i + 1]);

            if (keys[i + 1] < k)
                i++;
        }
        C[i + 1]->insertNonFull(k, data);
    }
}

template <typename KEY, typename DATA>
void TreeNode<KEY, DATA>::splitChild(int i, TreeNode<KEY, DATA> *y)
{
    TreeNode *z = new TreeNode(y->leaf);
    z->n = BTREE_DEGREE - 1;

    for (int j = 0; j < BTREE_DEGREE - 1; j++)
        z->keys[j] = y->keys[j + BTREE_DEGREE];

    if (y->leaf == false)
    {
        for (int j = 0; j < BTREE_DEGREE; j++)
            z->C[j] = y->C[j + BTREE_DEGREE];
    }

    y->n = BTREE_DEGREE - 1;
    for (int j = n; j >= i + 1; j--)
        C[j + 1] = C[j];

    C[i + 1] = z;

    for (int j = n - 1; j >= i; j--)
        keys[j + 1] = keys[j];

    keys[i] = y->keys[BTREE_DEGREE - 1];
    n = n + 1;
}

#endif