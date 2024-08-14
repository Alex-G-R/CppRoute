
#ifndef ROW_H
#define ROW_H

#include <fstream>
#include <vector>
#include <string>
#include <sstream>

class Row {
public:
    std::vector<std::string> values;

    Row(const std::vector<std::string>& values);

    std::string serialize() const;

    static Row deserialize(const std::string& str);
};


#endif //ROW_H
