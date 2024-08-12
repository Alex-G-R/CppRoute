
// Link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#include "Server.h"
#include "main.h"

std::string multiply(std::string req_body) {
    std::map<std::string, std::string> params = parseRequestBody(req_body);

    int num1 = std::stoi(params["num1"]);
    int num2 = std::stoi(params["num2"]);

    int result = num1 * num2;

    std::ostringstream response_stream;
    response_stream << "HTTP/1.1 200 OK\r\n";
    response_stream << "Content-Length: " << std::to_string(result).length() << "\r\n";
    response_stream << "Content-Type: text/plain\r\n\r\n";
    response_stream << result;

    return response_stream.str();
}

int main() {

    Server server{3000};

    // Serve home.html and it's required external goodies
    server.route("/", "/www/home.html");
    server.route("/home", "/www/home.html");
    server.route("/home.js", "/www/home.js");
    server.route("/styles.css", "/www/styles.css");

    server.post("/multiply", multiply);

    server.run_server();
    return 0;
}


// Helper function to parse the request body (application/x-www-form-urlencoded)
std::map<std::string, std::string> parseRequestBody(const std::string& req_body) {
    std::map<std::string, std::string> params;
    std::istringstream stream(req_body);
    std::string key_value;

    while (std::getline(stream, key_value, '&')) {
        size_t pos = key_value.find('=');
        if (pos != std::string::npos) {
            std::string key = key_value.substr(0, pos);
            std::string value = key_value.substr(pos + 1);
            params[key] = value;
        }
    }

    return params;
}

