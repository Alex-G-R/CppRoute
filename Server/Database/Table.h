
#ifndef TABLE_H
#define TABLE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <functional>
#include <sstream>

#include "Row.h"

class Table {
public:
    std::string name;
    std::vector<std::string> columns;
    std::vector<Row> rows;

    Table(const std::string& name, const std::vector<std::string>& columns);

    void insertRow(const std::vector<std::string>& values);

    std::vector<Row> select(std::function<bool(const Row&)> condition) const;

    void printTable() const;

    std::string serialize() const;

    static Table deserialize(const std::string& name, const std::string& str);
};


#endif //TABLE_H
