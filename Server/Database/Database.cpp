
#include "Database.h"
#include "../Server.h"

Database::Database(Server& server) : server(server) {}

void Database::createTable(const std::string& name, const std::vector<std::string>& columns) {
    tables.emplace(name, Table(name, columns));
}

void Database::insert(const std::string& tableName, const std::vector<std::string>& values) {
    auto it = tables.find(tableName);
    if (it != tables.end()) {
        it->second.insertRow(values);
    }
}

Table* Database::getTable(const std::string& tableName) {
    const auto it = tables.find(tableName);
    return it != tables.end() ? &(it->second) : nullptr;
}

std::vector<Row> Database::query(const std::string& tableName, std::function<bool(const Row&)> condition) {
    auto it = tables.find(tableName);
    if (it != tables.end()) {
        return it->second.select(std::move(condition));
    }
    return {};
}

void Database::printTable(const std::string& tableName) const {
    const auto it = tables.find(tableName);
    if (it != tables.end()) {
        it->second.printTable();
    }
}

void Database::saveToFile(const std::string& filename) {
    std::ofstream file(server.databases_dir+"/"+filename, std::ios::binary);
    for (const auto& [name, table] : tables) {
        file << name << "\n";
        file << table.serialize();
    }
}

void Database::loadFromFile(const std::string& filename) {
    std::ifstream file(server.databases_dir+"/"+filename, std::ios::binary);
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            std::string tableName = line;
            std::ostringstream tableData;
            while (std::getline(file, line) && !line.empty()) {
                tableData << line << "\n";
            }
            tables.emplace(tableName, Table::deserialize(tableName, tableData.str()));
        }
    }
}