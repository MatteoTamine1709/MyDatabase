# MyDatabase

## Description

This is a simple database that I created for my own use.
It is written in C++ from scratch and it is not using any external libraries aside from spdlog for logging and nlohmann_json for the config file.

It is implementing a BTree for indexing and a custom file format for storing the data.

It will be able to be used as a library in other projects as well as a standalone application that you can connect to via a TCP socket.

In the library, the tables will be known at compilation time and the user will be able to create, delete, insert, update and select data from the tables to gain extra performance.

In the standalone application, the tables will be known at runtime and the user will be able to create, delete, insert, update and select data from the tables. This will be slower than the library version because it will have to parse the table schema at runtime.

## Functionality

- [ ] Save data to disk
- [ ] Load data from disk
- [x] Save indexes to disk
- [x] Load indexes from disk
- [x] Create table
- [x] Delete table
- [x] Insert data
- [ ] Update data
- [x] Select data withut filters
- [x] Select data with filters
- [ ] Select data with filters and order by
- [ ] Select data with filters and order by and limit
- [x] Primary key
- [ ] Foreign key
- [x] Default values
- [x] Not null
- [x] Unique
- [ ] Auto increment
- [x] Indexes
- [ ] Composite indexes
- [ ] Transactions
- [ ] Views
- [ ] Stored procedures
- [ ] Stored functions
- [ ] Triggers
- [ ] User management
- [ ] Roles
- [ ] Permissions
- [ ] Replication?

## Database

- [ ] Config file
- [ ] Logging
- [ ] Documentation
- [ ] SQL parser

## Types

- [x] Integer
- [x] Float
- [x] Decimal
- [ ] Char (exist but not using the fixed size)
- [ ] Varchar (exist but not using the max size)
- [x] Text
- [x] Date
- [x] Time
- [x] Datetime
- [x] Boolean
- [x] Bit
- [ ] Blob
- [x] Varbinary (exist but not using the max size)
