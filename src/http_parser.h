#pragma once

#include <string>
#include <vector>

class HTTPParser
{
public:
    /// @breif splits on new line. First stage of http request decomp. Blank
    /// lines will be explicit as empty strings in the std::vector
    virtual auto split(std::string request) -> std::vector<std::string> = 0;
};
