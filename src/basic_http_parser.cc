#include <sstream>
#include <string>

#include "basic_http_parser.h"

auto BasicHTTPParser::split(std::string request) -> std::vector<std::string>
{
    std::vector<std::string> result{};
    std::stringstream ss{request};
    std::string current{};
    while (!ss.eof()) {
        std::getline(ss, current, '\n');
        result.push_back(current);
    }
    return result;
}

BasicHTTPParser::BasicHTTPParser() {}
