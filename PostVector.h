
#ifndef POSTVECTOR_H
#define POSTVECTOR_H

#include <string>
#include <functional>

class PostVector {
public:
        PostVector(std::string P_post_route, std::function<std::string(std::string req_body)> func);

        std::string post_route;
        std::function<std::string(std::string req_body)> handler_function;
};



#endif //POSTVECTOR_H
