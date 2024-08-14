
#include "Server.h"

Server::Server(const int P_PORT)
{
    this->port = P_PORT;
    running = true;
}

bool Server::ends_with(const std::string& str, const std::string& suffix) {
    if (str.size() < suffix.size()) {
        return false;
    }
    return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}

std::string Server::getSubstringAfterLastChar(const std::string& input, char specialChar) {
    // Find the position of the last occurrence of specialChar
    size_t pos = input.find_last_of(specialChar);

    // Check if the specialChar was found
    if (pos != std::string::npos) {
        // Extract the substring starting after the specialChar
        return input.substr(pos + 1);
    } else {
        // If specialChar is not found, return the whole input
        return input;
    }
}

// Helper function to parse the request body (application/x-www-form-urlencoded)
std::map<std::string, std::string> Server::parseRequestBody(const std::string& req_body) {
    std::map<std::string, std::string> params;
    std::istringstream stream(req_body);
    std::string key_value;

    while (std::getline(stream, key_value, '&')) {
        size_t pos = key_value.find('=');
        if (pos != std::string::npos) {
            std::string key = key_value.substr(0, pos);
            const std::string value = key_value.substr(pos + 1);
            params[key] = value;
        }
    }

    return params;
}

void Server::initialize_winsock() {
    WSADATA wsaData;
    if (const int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed: " << iResult << std::endl;
        exit(1);
    }
}

void Server::set_assets_routes(const std::string& P_assets_dir)
{
    std::vector<std::string> filenames;
    const std::filesystem::path folderPath = P_assets_dir;

    try
    {
        if(std::filesystem::exists(folderPath) && std::filesystem::is_directory(folderPath))
        {
            for(const auto& entry : std::filesystem::directory_iterator(folderPath))
            {
                if(entry.is_regular_file())
                {
                    filenames.push_back(entry.path().filename().string());
                }
            }
        }
        else
        {
            std::cerr << "This path does not exists or is not a directory." << std::endl;
        }
    } catch (const std::filesystem::filesystem_error& e)
    {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }

    // Add these assets to routes
    for (const auto& asset : filenames)
    {
        // Blame me for every bug out there... please refacor me..
        routing_vectors.emplace_back("/"+getSubstringAfterLastChar(folderPath.string(), '/')+"/"+asset, folderPath.string().erase(0, 1)+"/"+asset);
    }
}

void Server::set_views_dir(const std::string& P_views_dir)
{
    views_dir = P_views_dir;
}

void Server::set_pages_dir(const std::string& P_pages_dir)
{
    pages_dir = P_pages_dir;
}

void Server::add_session_variable(std::string P_name, std::string P_data_type, std::string P_value)
{
    session_variables.emplace_back(std::move(P_name), std::move(P_data_type), std::move(P_value));
}

void Server::render(std::string file_name) {
    const std::string startDelimiter = "<%=";
    const std::string endDelimiter = "=%>";
    std::regex pattern(startDelimiter + "(.*?)" + endDelimiter);

    std::string filename = views_dir + "/" + file_name;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open the file: " << filename << std::endl;
        return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string text = buffer.str();

    std::string modifiedText;
    std::sregex_iterator currentMatch(text.begin(), text.end(), pattern);
    std::sregex_iterator lastMatch;

    std::size_t lastPosition = 0;

    while (currentMatch != lastMatch) {
        std::smatch match = *currentMatch;

        // Append text before the match
        modifiedText += text.substr(lastPosition, match.position() - lastPosition);

        // Get the content between the delimiters
        std::string key = match.str(1);

        // Find the corresponding session variable value
        std::string replacement;
        auto it = std::find_if(session_variables.begin(), session_variables.end(),
            [&key](const SessionVariable& var) { return var.name == key; });

        if (it != session_variables.end()) {
            replacement = it->value;
        } else {
            // If no match is found in session_variables, leave the placeholder as is
            replacement = match.str();
        }

        // Replace the entire match (including delimiters) with the replacement string
        modifiedText += replacement;

        // Update the last position after the current match
        lastPosition = match.position() + match.length();

        // Move to the next match
        ++currentMatch;
    }

    // Append any remaining text after the last match
    modifiedText += text.substr(lastPosition);

    // Save HTML
    // Open the file
    std::string new_file_name = file_name.erase(file_name.length() - 5);
    new_file_name += "html";

    std::ofstream outFile(pages_dir+"/"+new_file_name);

    // Check if the file was successfully opened
    if (!outFile) {
        std::cerr << "Failed to create or open the file: " << views_dir+"/"+new_file_name << std::endl;
        return;
    }

    // Write the content to the file
    outFile << modifiedText;

    // Close the file (optional, as it will be closed automatically when outFile goes out of scope)
    outFile.close();
}


SOCKET Server::create_server_socket() const {
    const SOCKET server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        exit(1);
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Bind to any available interface
    server_addr.sin_port = htons(port); // Port 3000

    if (bind(server_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == SOCKET_ERROR) {
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

SOCKET Server::accept_client(const SOCKET server_socket) {
    const SOCKET client_socket = accept(server_socket, nullptr, nullptr);
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

std::string Server::handle_post_request(const std::string& request, const std::function<std::string(const std::string req_body)>& func) {
    const size_t content_start = request.find("\r\n\r\n") + 4;
    const std::string body = request.substr(content_start);

    return func(body);
}

void Server::run_server() {
    initialize_winsock();
    const SOCKET server_socket = create_server_socket();

    while (running) {
        const SOCKET client_socket = accept_client(server_socket);

        char buffer[1024];
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received > 0) {
            std::string request(buffer, bytes_received);
            std::string path = parse_http_request(request);
            std::string response;

            // Rounting

            // Check for server shutdown
            if(path == "/q")
                running = false;

            bool route_found = false;
            // Check routing_vectors
            for (const auto& route_info : routing_vectors) {
                if (path == route_info.route) {
                    response = serve_file(route_info.file_path);
                    route_found = true;
                    break;
                }
            }

            // If no route was found, check post_routes
            if (!route_found) {
                for (const auto& post_route_info : post_routes) {
                    if (request.find("POST " + post_route_info.post_route) != std::string::npos) {
                        response = handle_post_request(request, post_route_info.handler_function);
                        route_found = true;
                        break;
                    }
                }
            }

            // Default to serving files if no explicit route matches
            if (!route_found) {
                response = "HTTP/1.1 404 Not Found\r\n\r\nNot Found";
            }

            send(client_socket, response.c_str(), static_cast<int>(response.length()), 0);
        }

        closesocket(client_socket);
    }

    closesocket(server_socket);
    WSACleanup();
}

void Server::route(std::string P_route, std::string P_file_path) {
    routing_vectors.emplace_back(std::move(P_route), std::move(P_file_path));
}

void Server::post(std::string P_post_route, std::function<std::string(const std::string& req_body)> func)
{
    post_routes.emplace_back(std::move(P_post_route), std::move(func));
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