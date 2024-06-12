#pragma once

#include "http_parser.h"

namespace http
{

class ResponseHeader
{
private:
    ProtocolVersion protocol_version;
    int status_code;
    std::string status_message;
    std::vector<std::pair<std::string, std::string>> headers;

public:
    ResponseHeader();
    ResponseHeader(int status_code, std::string status_msg);
    auto to_string() const -> std::string;

    auto set_status_code(int status_code) -> void;
    auto set_status_message(std::string status_message) -> void;
    auto add_header(std::string name, std::string args) -> void;
};

} // namespace http
