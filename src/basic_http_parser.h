#pragma once

#include "http_parser.h"

namespace http
{
class BasicParser : public Parser
{
public:
    BasicParser();

    auto split(std::string request) const -> std::vector<std::string>;
    auto parse_request_line(std::string raw_request_line) const
        -> std::optional<
            std::tuple<RequestMethod, raw_request_target_t, ProtocolVersion>>;
};

} // namespace http
