#include "connection.h"

#include <iostream>
#include <unistd.h>

using namespace http;

Connection::Connection(int fd)
{
    live = fd >= 0;
    if (live) {
        confd = fd;
    }
}

Connection::~Connection()
{
    if (live) {
        close(confd);
    }
}

auto Connection::is_live() const -> bool { return live; }

auto Connection::read_line() -> std::string
{
    std::string line{};
    while (true && live) {
        char c;
        int n = read(confd, &c, 1);
        if (c == '\n') {
            break;
        } else {
            line.push_back(c);
        }
    }
    return line;
}

// reads until either "\n\n" or "\r\n\r\n" is found
auto Connection::read_header() -> std::string
{
    std::string line{};
    if (!live) {
        return line;
    }
    size_t constexpr buff_size = 4;
    char buffer[buff_size] = {0, 0, 0, 0};
    size_t ptr{0};
    while (true) {
        int n = read(confd, buffer + (ptr % 4), 1);
        if (n > 0) {
            ptr += n;
        }
        line.push_back(buffer[ptr % 4]);
        if (buffer[(ptr + 0) % 4] == '\r') {
            if (buffer[(ptr + 1) % 4] == '\n') {
                if (buffer[(ptr + 2) % 4] == '\r') {
                    if (buffer[(ptr + 3) % 4] == '\n') {
                        break;
                    }
                }
            }
        } else if (buffer[(ptr + 0) % 4] == '\n') {
            if (buffer[(ptr + 1) % 4] == '\n') {
                break;
            }
        }
    }
    return line;
}

auto Connection::write(std::string const& msg) -> void
{
    int n = ::write(confd, msg.c_str(), msg.size());
    if (n < 0) {
        std::cerr << "failed to write response in write connection\n";
    }
}
