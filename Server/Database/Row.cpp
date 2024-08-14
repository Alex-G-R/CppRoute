
#include "Row.h"



Row::Row(const std::vector<std::string>& values) : values(values) {}

std::string Row::serialize() const {
    std::ostringstream oss;
    for (const auto& value : values) {
        oss << value << "\t";
    }
    return oss.str();
}

Row Row::deserialize(const std::string& str) {
    std::istringstream iss(str);
    std::vector<std::string> values;
    std::string value;
    while (std::getline(iss, value, '\t')) {
        values.push_back(value);
    }
    return Row(values);
}
