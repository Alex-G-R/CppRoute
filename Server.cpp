
#include "Server.h"

Server::Server(int P_PORT)
{
    this->port = P_PORT;
}


void Server::initialize_winsock() {
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cerr << "WSAStartup failed: " << iResult << std::endl;
        exit(1);
    }
}

SOCKET Server::create_server_socket() {
    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        exit(1);
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Bind to any available interface
    server_addr.sin_port = htons(port); // Port 3000

    if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
        closesocket(server_socket);
        WSACleanup();
        exit(1);
    }

    if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
        closesocket(server_socket);
        WSACleanup();
        exit(1);
    }

    return server_socket;
}

SOCKET Server::accept_client(SOCKET server_socket) {
    SOCKET client_socket = accept(server_socket, NULL, NULL);
    if (client_socket == INVALID_SOCKET) {
        std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
        closesocket(server_socket);
        WSACleanup();
        exit(1);
    }
    return client_socket;
}

std::string Server::parse_http_request(const std::string& request) {
    std::istringstream request_stream(request);
    std::string method;
    std::string path;
    request_stream >> method >> path;

    return path;
}

std::string Server::serve_file(const std::string& path) {
    std::ifstream file("." + path, std::ios::binary);
    if (!file) {
        return "HTTP/1.1 404 Not Found\r\n\r\nFile Not Found";
    }

    std::ostringstream file_stream;
    file_stream << file.rdbuf();
    std::string file_content = file_stream.str();

    std::ostringstream response_stream;
    response_stream << "HTTP/1.1 200 OK\r\n";
    response_stream << "Content-Length: " << file_content.size() << "\r\n";
    response_stream << "Content-Type: text/html\r\n\r\n";
    response_stream << file_content;

    return response_stream.str();
}

std::string Server::handle_post_request(const std::string& request, std::function<std::string(std::string req_body)> func) {
    size_t content_start = request.find("\r\n\r\n") + 4;
    std::string body = request.substr(content_start);

    // int number = std::stoi(body); // Assuming the body contains just a number
    // int result = number * 4;

    // std::ostringstream response_stream;
    // response_stream << "HTTP/1.1 200 OK\r\n";
    // response_stream << "Content-Length: " << std::to_string(result).length() << "\r\n";
    // response_stream << "Content-Type: text/plain\r\n\r\n";
    // response_stream << result;

    return func(body);

    // return response_stream.str();
}

void Server::run_server() {
    initialize_winsock();
    SOCKET server_socket = create_server_socket();

    while (true) {
        SOCKET client_socket = accept_client(server_socket);

        char buffer[1024];
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received > 0) {
            std::string request(buffer, bytes_received);
            std::string path = parse_http_request(request);
            std::string response;

            // Rounting

            for(size_t i = 0; i < routing_vectors.size(); i++)
            {
                for(size_t k = 0; k < post_routes.size(); k++)
                if(path == routing_vectors[i].route)
                {
                    response = serve_file(routing_vectors[i].file_path);
                }
                else if (request.find("POST "+post_routes[k].post_route) != std::string::npos) {
                    response = handle_post_request(request, post_routes[k].handler_function);
                } else {
                    response = "HTTP/1.1 404 Not Found\r\n\r\nNot Found";
                }
            }


            send(client_socket, response.c_str(), response.length(), 0);
        }

        closesocket(client_socket);
    }

    closesocket(server_socket);
    WSACleanup();
}

void Server::route(std::string P_route, std::string P_file_path) {
    routing_vectors.emplace_back(RoutingVector(P_route, P_file_path));
}

void Server::post(std::string P_post_route, std::function<std::string(std::string req_body)> func)
{
    post_routes.emplace_back(PostVector(P_post_route, func));
}
