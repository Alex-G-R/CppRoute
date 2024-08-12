
#include "Server.h"

Server::Server(int P_PORT)
{
    this->port = P_PORT;
}

/* To do? Meaby -> Don't yet know if really needed or bloaty
void Server::set_paths(std::string P_pages_path, std::string P_assets_path, std::string P_styles_path, std::string P_scripts_path)
{
    pages_folder = P_pages_path;
    assets_folder = P_assets_path;
    styles_folder = P_styles_path;
    scripts_folder = P_scripts_path;
}
*/

bool Server::ends_with(const std::string& str, const std::string& suffix) {
    if (str.size() < suffix.size()) {
        return false;
    }
    return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
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

    std::string content_type = get_content_type(path);

    std::ostringstream response_stream;
    response_stream << "HTTP/1.1 200 OK\r\n";
    response_stream << "Content-Length: " << file_content.size() << "\r\n";
    response_stream << "Content-Type: " << content_type << "\r\n\r\n";
    response_stream << file_content;

    return response_stream.str();
}

std::string Server::handle_post_request(const std::string& request, std::function<std::string(std::string req_body)> func) {
    // std::cout << request << std::endl;
    size_t content_start = request.find("\r\n\r\n") + 4;
    std::string body = request.substr(content_start);

    return func(body);
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

            bool route_found = false;
            for (size_t i = 0; i < routing_vectors.size(); i++) {
                if (path == routing_vectors[i].route) {
                    response = serve_file(routing_vectors[i].file_path);
                    route_found = true;
                    break;
                }
            }

            if (!route_found) {
                for (size_t k = 0; k < post_routes.size(); k++) {
                    if (request.find("POST " + post_routes[k].post_route) != std::string::npos) {
                        response = handle_post_request(request, post_routes[k].handler_function);
                        route_found = true;
                        break;
                    }
                }
            }

            // Default to serving files if no explicit route matches
            if (!route_found) {
                response = "HTTP/1.1 404 Not Found\r\n\r\nNot Found";
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

std::string Server::get_content_type(const std::string& path)
{
    // Default to HTML
    std::string content_type;

    if (ends_with(path, ".css")) {
        content_type = "text/css";
    }
    else if (ends_with(path, ".js")) {
        content_type = "application/javascript";
    }
    else if (ends_with(path, ".html") || ends_with(path, ".htm")) {
        content_type = "text/html";
    }
    else if (ends_with(path, ".jpg") || ends_with(path, ".jpeg")) {
        content_type = "image/jpeg";
    }
    else if (ends_with(path, ".png")) {
        content_type = "image/png";
    }
    else if (ends_with(path, ".gif")) {
        content_type = "image/gif";
    }
    else if (ends_with(path, ".ico")) {
        content_type = "image/x-icon";
    }
    else if (ends_with(path, ".svg")) {
        content_type = "image/svg+xml";
    }
    else if (ends_with(path, ".json")) {
        content_type = "application/json";
    }
    else if (ends_with(path, ".xml")) {
        content_type = "application/xml";
    }
    else if (ends_with(path, ".pdf")) {
        content_type = "application/pdf";
    }
    else if (ends_with(path, ".zip")) {
        content_type = "application/zip";
    }
    else if (ends_with(path, ".txt")) {
        content_type = "text/plain";
    }
    else if (ends_with(path, ".csv")) {
        content_type = "text/csv";
    }
    else if (ends_with(path, ".mp3")) {
        content_type = "audio/mpeg";
    }
    else if (ends_with(path, ".mp4")) {
        content_type = "video/mp4";
    }
    else if (ends_with(path, ".webm")) {
        content_type = "video/webm";
    }
    else if (ends_with(path, ".ogg")) {
        content_type = "audio/ogg";
    }
    else if (ends_with(path, ".wav")) {
        content_type = "audio/wav";
    }
    else if (ends_with(path, ".avi")) {
        content_type = "video/x-msvideo";
    }
    else if (ends_with(path, ".woff")) {
        content_type = "font/woff";
    }
    else if (ends_with(path, ".woff2")) {
        content_type = "font/woff2";
    }
    else if (ends_with(path, ".ttf")) {
        content_type = "font/ttf";
    }
    else if (ends_with(path, ".eot")) {
        content_type = "application/vnd.ms-fontobject";
    }
    else if (ends_with(path, ".otf")) {
        content_type = "font/otf";
    }
    else {
        content_type = "application/octet-stream";
    }

    return content_type;
}