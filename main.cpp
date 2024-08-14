// Link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#include "Server/Server.h"
#include "functions.h"

int main() {

    // Start by creating a server ;)
    
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
            const std::string value = key_value.substr(pos + 1);
            params[key] = value;
        }
    }

    return params;
}
