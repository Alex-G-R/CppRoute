
#ifndef SERVER_H
#define SERVER_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <functional>

#include "RoutingVector.h"
#include "PostVector.h"

// Link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

class Server {
public:
    Server(int P_PORT);
    void run_server();
    void handle_route(RoutingVector P_route);
    void handle_post(PostVector P_post_handler);

private:
    int port;

    std::vector<RoutingVector> routing_vectors;
    std::vector<PostVector> post_routes;

    void initialize_winsock();
    SOCKET create_server_socket();
    SOCKET accept_client(SOCKET server_socket);
    std::string parse_http_request(const std::string& request);
    std::string serve_file(const std::string& path);
    std::string handle_post_request(const std::string& request, std::function<std::string(std::string req_body)> func);
};



#endif //SERVER_H
