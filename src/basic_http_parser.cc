#include <optional>
#include <sstream>
#include <string>

#include "basic_http_parser.h"
#include "http_parser.h"

using namespace http;

http::BasicParser::BasicParser() {}

auto http::BasicParser::split(std::string request) const
    -> std::vector<std::string>
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

auto http::BasicParser::parse_request_line(std::string raw_request_line) const
    -> std::optional<
        std::tuple<RequestMethod, raw_request_target_t, ProtocolVersion>>
{
    std::stringstream ss{raw_request_line};
    std::string store{};

    RequestMethod request_method{};
    ss >> store;
    auto const asciitolower = [](char in) -> char {
        if (in <= 'Z' && in >= 'A') {
            return in - ('Z' - 'z');
        }
        return in;
    };

    std::transform(store.begin(), store.end(), store.begin(), asciitolower);
    if (store == "get") {
        request_method = RequestMethod::Get;
    } else if (store == "head") {
        request_method = RequestMethod::Head;
    } else if (store == "post") {
        request_method = RequestMethod::Post;
    } else if (store == "put") {
        request_method = RequestMethod::Put;
    } else if (store == "delete") {
        request_method = RequestMethod::Delete;
    } else if (store == "connect") {
        request_method = RequestMethod::Connect;
    } else if (store == "options") {
        request_method = RequestMethod::Options;
    } else if (store == "trace") {
        request_method = RequestMethod::Trace;
    } else if (store == "patch") {
        request_method = RequestMethod::Patch;
    } else {
        return {};
    }

    raw_request_target_t target{};
    ss >> target;

    ProtocolVersion pv{};
    store.clear();
    ss >> store;
    if (store == "HTTP/1.1") {
        pv = ProtocolVersion::OnePointOne;
    } else if (store == "HTTP/1.0") {
        pv = ProtocolVersion::One;
    } else {
        return {};
    }

    std::tuple<RequestMethod, raw_request_target_t, ProtocolVersion> const
        result{request_method, target, pv};
    return result;
}

auto BasicParser::parse_header(std::string raw_header) const
    -> std::optional<std::pair<std::string, std::string>>
{
    auto colon_pos = raw_header.find_first_of(':');
    if (colon_pos == std::string::npos) {
        return {};
    }
    std::string first{raw_header.substr(0, colon_pos)};
    std::string second{raw_header.substr(colon_pos + 1)};
    std::pair<std::string, std::string> result{first, second};
    return result;
}

auto BasicParser::parse_target(raw_request_target_t raw_target) const
    -> std::pair<std::string, std::optional<std::string>>
{
    auto const question_mark_pos{raw_target.find_first_of('?')};
    if (question_mark_pos != std::string::npos) {
        return {
            raw_target.substr(0, question_mark_pos),
            raw_target.substr(question_mark_pos + 1)
        };
    } else {
        return {raw_target, {}};
    }
}
