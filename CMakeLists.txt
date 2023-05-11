cmake_minimum_required(VERSION 3.0.0)
project(MyDatabase VERSION 0.1.0)

enable_testing()

# DEBUG
set(CMAKE_BUILD_TYPE Debug)
add_executable(MyDatabase src/main.cpp)


find_package(Threads REQUIRED)
find_package(spdlog REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)

target_link_libraries(MyDatabase PRIVATE Threads::Threads spdlog::spdlog nlohmann_json::nlohmann_json)