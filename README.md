# MyDatabase

## Description

This is a simple database that I created for my own use.
It is written in C++ from scratch and it is not using any external libraries aside from spdlog for logging and nlohmann_json for the config file.

It is implementing a BTree for indexing and a custom file format for storing the data.

It will be able to be used as a library in other projects as well as a standalone application that you can connect to via a TCP socket.

In the library, the tables will be known at compilation time and the user will be able to create, delete, insert, update and select data from the tables to gain extra performance.

In the standalone application, the tables will be known at runtime and the user will be able to create, delete, insert, update and select data from the tables. This will be slower than the library version because it will have to parse the table schema at runtime.
