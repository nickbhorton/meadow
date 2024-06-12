#include <sstream>
#include <string>

#include "basic_http_parser.h"

auto BasicHTTPParser::split(std::string request) -> std::vector<std::string>
{
    std::vector<std::string> result{};
    std::istringstream ss{request};
    std::string current{};
    bool parse_payload = false;
    unsigned int loop_count = 0;
    while (!ss.eof()) {
        std::getline(ss, current);
        if (current.size() == 0 ||
            (current.size() == 1 && current[0] == '\r')) {
            parse_payload = true;
            break;
        } else if (current.at(current.size() - 1) == '\r') {
            result.push_back(current.substr(0, current.size() - 1));
        } else {
            result.push_back(current);
        }
        ++loop_count;
    }
    if (parse_payload) {
        std::stringstream tmp;
        tmp << ss.rdbuf();
        current = tmp.str();
        if (current.size() > 0) {
            if (loop_count == 0) {
                // for if there is no header remove the first \n
                if (current[0] == '\r') {
                    current = current.substr(2);
                } else if (current[0] == '\n') {
                    current = current.substr(1);
                }
            }
            result.push_back(current);
        }
    }
    return result;
}

BasicHTTPParser::BasicHTTPParser() {}
