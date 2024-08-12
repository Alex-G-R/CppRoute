
#ifndef SERVER_H
#define SERVER_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
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
    Server(int P_PORT);
    void run_server();

    // To do? Meaby -> Don't yet know if really needed or bloaty
    // void set_paths(std::string P_pages_path, std::string P_assets_path, std::string P_styles_path, std::string P_scripts_path);

    void route(std::string P_route, std::string P_file_path);
    void post(std::string P_post_route, std::function<std::string(std::string req_body)> func);

private:
    int port;

    std::vector<RoutingVector> routing_vectors;
    std::vector<PostVector> post_routes;

    // To do? Meaby -> Don't yet know if really needed or bloaty
    // td::string assets_folder;
    // td::string pages_folder;
    // td::string scripts_folder;
    // td::string styles_folder;

    bool ends_with(const std::string& str, const std::string& suffix);
    std::string get_content_type(const std::string& path);

    void initialize_winsock();
    SOCKET create_server_socket();
    SOCKET accept_client(SOCKET server_socket);
    std::string parse_http_request(const std::string& request);
    std::string serve_file(const std::string& path);
    std::string handle_post_request(const std::string& request, std::function<std::string(std::string req_body)> func);
};



#endif //SERVER_H
