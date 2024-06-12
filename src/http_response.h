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

public:
    ResponseHeader();
    auto to_string() const -> std::string;

    auto set_status_code(int status_code) -> void;
    auto set_status_message(std::string status_message) -> void;
};

} // namespace http
