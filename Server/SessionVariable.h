
#ifndef SESSIONVARIABLE_H
#define SESSIONVARIABLE_H

#include <string>

class SessionVariable {
public:
    SessionVariable(std::string P_name, std::string P_data_type, std::string P_value);

    std::string name;
    std::string data_type;
    std::string value;
};



#endif //SESSIONVARIABLE_H
