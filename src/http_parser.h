#pragma once

#include <optional>
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
