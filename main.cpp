// Link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#include "Server/Server.h"
#include "functions.h"

int main() {
    std::cout << "1" << std::endl;
    Server server{3000};
    std::cout << "2" << std::endl;

    server.set_pages_dir("./www");
    std::cout << "3" << std::endl;

    // Serve math.html
    server.route("/math", "/www/math.html");
    server.route("/math.js", "/www/math.js");
    server.post("/multiplyThree", multiplyThree);
    std::cout << "4" << std::endl;

    server.run_server();
    std::cout << "Server is running on PORT: 3000" << std::endl;
    return 0;
}

std::string multiplyThree(const std::string& req_body) {
    std::map<std::string, std::string> params = Server::parseUrlencodedRequestBody(req_body);

    const int num1 = std::stoi(params["num1"]);
    const int num2 = std::stoi(params["num2"]);
    const int num3 = std::stoi(params["num3"]);

    const int result = num1 * num2 * num3;

    std::ostringstream response_stream;
    response_stream << "HTTP/1.1 200 OK\r\n";
    response_stream << "Content-Length: " << std::to_string(result).length() << "\r\n";
    response_stream << "Content-Type: text/plain\r\n\r\n";
    response_stream << result;

    return response_stream.str();
}