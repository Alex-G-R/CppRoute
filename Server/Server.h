
#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <winsock2.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <functional>
#include <unordered_map>

#include "RoutingVector.h"
#include "PostVector.h"

// Link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

class Server {
public:
    explicit Server(int P_PORT);
    void run_server();

    void route(std::string P_route, std::string P_file_path);
    void post(std::string P_post_route, std::function<std::string(const std::string& req_body)> func);

private:
    int port;
    bool running;

    std::vector<RoutingVector> routing_vectors;
    std::vector<PostVector> post_routes;

    static bool ends_with(const std::string& str, const std::string& suffix);
    static std::string get_content_type(const std::string& path);

    static void initialize_winsock();
    [[nodiscard]] SOCKET create_server_socket() const;
    static SOCKET accept_client(SOCKET server_socket);
    static std::string parse_http_request(const std::string& request);
    static std::string serve_file(const std::string& path);
    static std::string handle_post_request(const std::string& request, const std::function<std::string(const std::string req_body)>& func);
};



#endif //SERVER_H
