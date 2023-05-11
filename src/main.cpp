#include "BTree.h"
#include <iostream>

struct K
{
    int value = 0;
    bool operator>(const K &k) const { return value > k.value; }
    bool operator<(const K &k) const { return value < k.value; }
    bool operator==(const K &k) const { return value == k.value; }
    // << operator
    friend std::ostream &operator<<(std::ostream &os, const K &k)
    {
        os << "=> " << k.value;
        return os;
    }
    K(int value) : value(value) {}
    K() = default;
};

int main()
{
    BTree<K, const char *> t;
    t.insert(K(9), "World");
    t.insert(8, "Hello");
    t.insert(10, "!");
    t.insert(11, "This");
    t.insert(15, "is");
    t.insert(16, "a");
    t.insert(17, "B-tree");
    t.insert(18, "implementation");
    t.insert(20, "in");
    t.insert(23, "C++");

    std::cout << "The B-tree is: ";
    t.traverse();

    int k = 10;
    (t.search(k) != nullptr) ? std::cout << std::endl
                                         << k << " is found" << std::endl
                             : std::cout << std::endl
                                         << k << " is not Found" << std::endl;

    std::cout << t.search(8)->data << std::endl;
}
