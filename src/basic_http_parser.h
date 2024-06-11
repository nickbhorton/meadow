#pragma once

#include "http_parser.h"

class BasicHTTPParser : public HTTPParser
{
public:
    BasicHTTPParser();

    auto split(std::string request) -> std::vector<std::string>;
};
