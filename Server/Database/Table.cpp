
#include "Table.h"

Table::Table(const std::string& name, const std::vector<std::string>& columns)
        : name(name), columns(columns) {}

void Table::insertRow(const std::vector<std::string>& values) {
    rows.emplace_back(values);
}

std::vector<Row> Table::select(std::function<bool(const Row&)> condition) const {
    std::vector<Row> result;
    for (const auto& row : rows) {
        if (condition(row)) {
            result.push_back(row);
        }
    }
    return result;
}

void Table::printTable() const {
    for (const auto& col : columns) {
        std::cout << col << "\t";
    }
    std::cout << std::endl;

    for (const auto& row : rows) {
        std::cout << row.serialize() << std::endl;
    }
}

std::string Table::serialize() const {
    std::ostringstream oss;
    for (const auto& col : columns) {
        oss << col << "\t";
    }
    oss << "\n";

    for (const auto& row : rows) {
        oss << row.serialize() << "\n";
    }
    return oss.str();
}

Table Table::deserialize(const std::string& name, const std::string& str) {
    std::istringstream iss(str);
    std::string line;
    std::getline(iss, line);

    std::vector<std::string> columns;
    std::istringstream columnStream(line);
    std::string column;
    while (std::getline(columnStream, column, '\t')) {
        columns.push_back(column);
    }

    Table table(name, columns);
    while (std::getline(iss, line)) {
        if (!line.empty()) {
            table.rows.push_back(Row::deserialize(line));
        }
    }
    return table;
}