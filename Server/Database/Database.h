
#ifndef DATABASE_H
#define DATABASE_H

#include <vector>
#include <string>
#include <unordered_map>

// Forward declare the Server class
class Server;

#include "Table.h"

class Database {
private:
    std::unordered_map<std::string, Table> tables;
    Server& server;

public:
    explicit Database(Server& server);

    void createTable(const std::string& name, const std::vector<std::string>& columns);

    void insert(const std::string& tableName, const std::vector<std::string>& values);

    Table* getTable(const std::string& tableName);

    std::vector<Row> query(const std::string& tableName, std::function<bool(const Row&)> condition);

    void printTable(const std::string& tableName) const;

    void saveToFile(const std::string& filename);

    void loadFromFile(const std::string& filename);
};



#endif //DATABASE_H
