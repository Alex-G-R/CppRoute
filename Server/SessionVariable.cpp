
#include "SessionVariable.h"

SessionVariable::SessionVariable(std::string P_name, std::string P_data_type, std::string P_value)
{
    name = std::move(P_name);
    data_type = std::move(P_data_type);
    value = std::move(P_value);
}