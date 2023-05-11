#include "BTree.h"
#include <iostream>
#include <vector>
#include <chrono>

struct USER
{
    int id;
    std::string name;
    USER(int id, std::string name) : id(id), name(name) {}
};

void bench_user_id_index(std::vector<USER *> &users)
{
    BTree<decltype(USER::id), USER *> USER_ID_INDEX;
    std::cout << "inserting..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    for (auto user : users)
    {
        USER_ID_INDEX.insert(user->id, user);
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << duration.count() << std::endl;

    std::cout << "removing..." << std::endl;
    start = std::chrono::high_resolution_clock::now();
    for (int i = (users.size() / 2) - (users.size() / 4); i < users.size() / 2; ++i)
        USER_ID_INDEX.remove(i);
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << duration.count() << std::endl;

    std::cout << "searching..." << std::endl;
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < users.size(); ++i)
    {
        auto [key, data] = USER_ID_INDEX.search(i);
    }
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << duration.count() << std::endl;

    if (users.size() < 1000)
    {
        std::cout << "printing..." << std::endl;
        USER_ID_INDEX.traverse();
    }

    std::cout << "clearing..." << std::endl;
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < users.size(); ++i)
    {
        USER_ID_INDEX.remove(i);
    }
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << duration.count() << std::endl;
}

void bench_user_name_index(std::vector<USER *> &users)
{
    BTree<decltype(USER::name), USER *> USER_NAME_INDEX;
    std::cout << "inserting..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    for (auto user : users)
    {
        USER_NAME_INDEX.insert(user->name, user);
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << duration.count() << std::endl;

    std::cout << "removing..." << std::endl;
    start = std::chrono::high_resolution_clock::now();
    for (int i = (users.size() / 2) - (users.size() / 4); i < users.size() / 2; ++i)
        USER_NAME_INDEX.remove("user" + std::to_string(i));
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << duration.count() << std::endl;

    std::cout << "searching..." << std::endl;
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < users.size(); ++i)
    {
        auto [key, data] = USER_NAME_INDEX.search("user" + std::to_string(i));
    }
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << duration.count() << std::endl;
    if (users.size() < 1000)
    {
        std::cout << "printing..." << std::endl;
        USER_NAME_INDEX.traverse();
    }

    std::cout << "clearing..." << std::endl;
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < users.size(); ++i)
    {
        USER_NAME_INDEX.remove(users[i]->name);
    }
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << duration.count() << std::endl;
}

int main()
{
    std::vector<USER *> users;
    for (int i = 0; i < 100; i++)
    {
        users.push_back(new USER(i, "user" + std::to_string(i)));
    }
    bench_user_id_index(users);

    // bench_user_name_index(users);
}
