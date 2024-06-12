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
ResponseHeader::ResponseHeader(int status_code, std::string status_msg)
{
    this->status_code = status_code;
    status_message = status_msg;
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
    ss << status_code << " " << status_message << "\r\n";
    // automatic header
    ss << "Server: Meadow\r\n";
    for (auto const& [name, args] : headers) {
        ss << name << ": " << args << "\r\n";
    }
    ss << "\r\n";
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

auto ResponseHeader::add_header(std::string name, std::string args) -> void
{
    headers.push_back({name, args});
}
