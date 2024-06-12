#include "http_response.h"
#include "http_parser.h"
#include <sstream>

using namespace http;

ResponseHeader::ResponseHeader()
{
    status_code = 501;
    status_message = "Not Implemented";
    protocol_version = ProtocolVersion::OnePointOne;
}

auto ResponseHeader::to_string() const -> std::string
{
    std::stringstream ss;
    if (protocol_version == ProtocolVersion::One) {
        ss << "HTTP/1.0 ";
    } else {
        ss << "HTTP/1.1 ";
    }
    ss << status_code << " " << status_message;
    ss << "\r\n\r\n";
    return ss.str();
}

auto ResponseHeader::set_status_code(int status_code) -> void
{
    this->status_code = status_code;
}

auto ResponseHeader::set_status_message(std::string status_message) -> void
{
    this->status_message = status_message;
}
