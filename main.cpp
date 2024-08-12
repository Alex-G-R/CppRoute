
// Link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#include "Server.h"
#include "RoutingVector.h"

std::string multiply(std::string req_body)
{
    int number = std::stoi(req_body);
    int result = number * 4;

    std::ostringstream response_stream;
    response_stream << "HTTP/1.1 200 OK\r\n";
    response_stream << "Content-Length: " << std::to_string(result).length() << "\r\n";
    response_stream << "Content-Type: text/plain\r\n\r\n";
    response_stream << result;

    return response_stream.str();
}

int main() {

    Server server{3000};

    server.handle_route(RoutingVector("/home", "/www/home.html"));

    server.handle_post(PostVector("/multiply", multiply));

    server.run_server();
    return 0;
}
