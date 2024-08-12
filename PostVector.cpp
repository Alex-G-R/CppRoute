
#include "PostVector.h"

PostVector::PostVector(std::string P_post_route, std::function<std::string(std::string req_body)> func)
{
    post_route = P_post_route;
    handler_function = func;
}
