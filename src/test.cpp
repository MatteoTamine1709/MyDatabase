#include "BTree.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <unordered_map>

struct TABLE_COLUMN
{
    std::string name;
    std::string type;
    bool is_primary_key;
    bool is_unique;
    bool is_not_null;
    bool is_foreign_key;
    std::string foreign_key_table;
    std::string foreign_key_column;
    std::string default_value;
};

struct TABLE
{
    std::string name;
    std::vector<TABLE_COLUMN> columns;
    BTree<void *, void *> *primary_key_index;
    std::unordered_map<std::string, BTree<void *, void *> *> unique_index;
    std::unordered_map<std::string, BTree<void *, void *> *> foreign_key_index;
};

typedef std::string VARCHAR;
struct USER
{
    int id;
    VARCHAR name;
    USER(int id, VARCHAR name) : id(id), name(name) {}
    ~USER() = default;
};

void bench_user_id_index(int count)
{
    BTree<decltype(USER::id), USER> USER_ID_INDEX;
    std::cout << "inserting..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < count; ++i)
    {
        USER_ID_INDEX.insert(i, USER(i, "user" + std::to_string(i)));
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << duration.count() << std::endl;

    USER_ID_INDEX.prettyPrint();

    std::cout << "removing..." << std::endl;
    start = std::chrono::high_resolution_clock::now();
    for (int i = (count / 2) - (count / 4); i < count / 2; ++i)
    {
        std::cout << i << std::endl;
        USER_ID_INDEX.remove(i);
    }
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << duration.count() << std::endl;

    std::cout << "searching..." << std::endl;
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < count; ++i)
    {
        auto [key, data] = USER_ID_INDEX.search(i);
    }
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << duration.count() << std::endl;

    USER_ID_INDEX.prettyPrint();

    std::cout << "clearing..." << std::endl;
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < count; ++i)
    {
        USER_ID_INDEX.remove(i);
    }
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << duration.count() << std::endl;
}

void bench_user_name_index(int count)
{
    BTree<decltype(USER::name), USER> USER_NAME_INDEX;
    std::cout << "inserting..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < count; ++i)
    {
        USER_NAME_INDEX.insert("user" + std::to_string(i), USER(i, "user" + std::to_string(i)));
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << duration.count() << std::endl;

    USER_NAME_INDEX.traverse();
    std::cout << "printing..." << std::endl;
    USER_NAME_INDEX.prettyPrint();

    // USER_NAME_INDEX.traverse();
    std::cout << "removing..." << std::endl;
    start = std::chrono::high_resolution_clock::now();
    // USER_NAME_INDEX.remove("user" + std::to_string(10));
    for (int i = (count / 2) - (count / 4); i < count / 2; ++i)
        USER_NAME_INDEX.remove("user" + std::to_string(i));
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << duration.count() << std::endl;

    USER_NAME_INDEX.traverse();

    std::cout << "searching..." << std::endl;
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < count; ++i)
    {
        auto [key, data] = USER_NAME_INDEX.search("user" + std::to_string(i));
    }
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << duration.count() << std::endl;
    if (count < 1000)
    {
        std::cout << "printing..." << std::endl;
        USER_NAME_INDEX.traverse();
    }

    USER_NAME_INDEX.prettyPrint();
    std::cout << "clearing..." << std::endl;
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < count; ++i)
    {
        USER_NAME_INDEX.remove("user" + std::to_string(i));
    }
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << duration.count() << std::endl;

    USER_NAME_INDEX.prettyPrint();
}

int main()
{
    // for (int i = 0; i < 100; i++)
    // {
    //     users.push_back(new USER(i, "user" + std::to_string(i)));
    // }
    // bench_user_id_index(100);

    bench_user_name_index(100);
}
