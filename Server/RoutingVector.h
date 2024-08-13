
#ifndef ROUTINGVECTOR_H
#define ROUTINGVECTOR_H

#include <string>

class RoutingVector {
public:
    RoutingVector(std::string P_route, std::string P_file_path);

    std::string route;
    std::string file_path;
};



#endif //ROUTINGVECTOR_H
