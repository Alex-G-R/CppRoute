
#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <winsock2.h>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <map>
#include <regex>
#include <functional>
#include <unordered_map>

#include "Database/Database.h"

#include "RoutingVector.h"
#include "PostVector.h"
#include "SessionVariable.h"

// Link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

class Server {
public:
    explicit Server(int P_PORT);
    void run_server();

    Database db;

    void set_assets_routes(const std::string& P_assets_dir);

    void set_views_dir(const std::string& P_views_dir);
    void set_pages_dir(const std::string& P_pages_dir);

    void route(std::string P_route, std::string P_file_path);
    void post(std::string P_post_route, std::function<std::string(const std::string& req_body)> func);

    void add_session_variable(std::string P_name, std::string P_data_type, std::string P_value);

    void render(std::string file_name);

private:
    int port;
    bool running;

    std::string pages_dir;
    std::string views_dir;

    std::vector<RoutingVector> routing_vectors;
    std::vector<PostVector> post_routes;
    std::vector<SessionVariable> session_variables;

    static bool ends_with(const std::string& str, const std::string& suffix);
    static std::string getSubstringAfterLastChar(const std::string& input, char specialChar);
    static std::string get_content_type(const std::string& path);

    static void initialize_winsock();
    [[nodiscard]] SOCKET create_server_socket() const;
    static SOCKET accept_client(SOCKET server_socket);
    static std::string parse_http_request(const std::string& request);
    static std::string serve_file(const std::string& path);
    static std::string handle_post_request(const std::string& request, const std::function<std::string(const std::string req_body)>& func);
};



#endif //SERVER_H
