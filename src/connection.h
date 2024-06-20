#pragma once

#include <string>

namespace http
{

class Connection
{
    bool live;
    int confd;

public:
    Connection(int fd);
    Connection() = delete;

    // rule of 5
    ~Connection();
    Connection(Connection const& other) = delete;
    Connection(Connection&& other) noexcept = delete;
    Connection& operator=(Connection const& other) = delete;
    Connection& operator=(Connection&& other) noexcept = delete;

    auto is_live() const -> bool;
    auto read_line() -> std::string;
    auto read_header() -> std::string;
    auto write(std::string const& msg) -> void;
    auto read(size_t amount) -> std::string;
};

} // namespace http
