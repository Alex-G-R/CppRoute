
#include "RoutingVector.h"

RoutingVector::RoutingVector(std::string P_route, std::string P_file_path)
{
    route = std::move(P_route);
    file_path = std::move(P_file_path);
}
