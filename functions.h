
#ifndef MAIN_H
#define MAIN_H

std::map<std::string, std::string> parseRequestBody(const std::string& req_body);

std::string multiplyTwo(const std::string& req_body);
std::string multiplyThree(const std::string& req_body);

#endif //MAIN_H
