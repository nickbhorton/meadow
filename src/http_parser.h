#pragma once

#include <optional>
#include <ostream>
#include <string>
#include <vector>

namespace http
{

enum class RequestMethod {
    Get,
    Head,
    Post,
    Put,
    Delete,
    Connect,
    Options,
    Trace,
    Patch
};

enum class ProtocolVersion { One, OnePointOne };

typedef std::string raw_request_target_t;

class Parser
{
public:
    virtual ~Parser(){};
    /// @breif splits on new line. First stage of http request decomp. After an
    /// empty line the rest of the string will be streated as the payload and
    /// not split.
    virtual auto split(std::string request) const
        -> std::vector<std::string> = 0;

    virtual auto
    parse_request_line(std::string raw_request_line) const -> std::optional<
        std::tuple<RequestMethod, raw_request_target_t, ProtocolVersion>> = 0;

    virtual auto parse_header(std::string raw_header) const
        -> std::optional<std::pair<std::string, std::string>> = 0;

    virtual auto parse_target(raw_request_target_t raw_target) const
        -> std::pair<std::string, std::optional<std::string>> = 0;
};

} // namespace http

inline auto operator<<(std::ostream& os, http::RequestMethod rqm)
    -> std::ostream&
{
    switch (rqm) {

    case http::RequestMethod::Get:
        os << "GET";
        break;
    case http::RequestMethod::Head:
        os << "HEAD";
        break;
    case http::RequestMethod::Post:
        os << "POST";
        break;
    case http::RequestMethod::Put:
        os << "PUT";
        break;
    case http::RequestMethod::Delete:
        os << "DELETE";
        break;
    case http::RequestMethod::Connect:
        os << "CONNECT";
        break;
    case http::RequestMethod::Options:
        os << "OPTIONS";
        break;
    case http::RequestMethod::Trace:
        os << "TRACE";
        break;
    case http::RequestMethod::Patch:
        os << "PATCH";
        break;
    }
    return os;
}
