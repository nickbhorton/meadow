#include "client.h"

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <unistd.h>

constexpr size_t buffer_size = 64;

TcpClient::TcpClient()
{
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "socket() failed TcpClient: " << std::strerror(errno)
                  << "\n";
        return;
    }
}
TcpClient::~TcpClient() { close(fd); }

auto TcpClient::connect(std::string const& address, int port) -> bool
{
    sockaddr_in server_address{};

    server_address.sin_family = AF_INET; // IPv4
    inet_pton(
        server_address.sin_family,
        address.c_str(),
        &server_address.sin_addr
    );
    server_address.sin_port = htons(port);
    if (::connect(
            fd,
            (struct sockaddr*)&server_address,
            sizeof(server_address)
        ) < 0) {
        connected = false;
    } else {
        connected = true;
    }
    return connected;
}

auto TcpClient::write(std::string const& mesg) -> int
{
    if (!connected) {
        return -1;
    }
    size_t string_size = mesg.size();
    int total_bytes_sent{0};
    int current_bytes_sent =
        ::write(fd, (const char*)&string_size, sizeof(string_size));
    if (current_bytes_sent < 0) {
        std::cerr << "write() failed TcpClient: " << std::strerror(errno)
                  << "\n";
        return -1;
    }
    total_bytes_sent += current_bytes_sent;

    size_t string_pos{0};
    while (string_pos + buffer_size < mesg.size()) {
        int current_bytes_send =
            ::write(fd, mesg.data() + string_pos, sizeof(buffer_size));
        if (current_bytes_send < 0) {
            std::cerr << "write() failed TcpClient: " << std::strerror(errno)
                      << "\n";
            return -1;
        }
        string_pos += current_bytes_send;
    }
    int bytes_left = mesg.size() - string_pos;
    if (bytes_left > 0) {
        int current_bytes_send =
            ::write(fd, mesg.data() + string_pos, bytes_left);
        if (current_bytes_send < 0) {
            std::cerr << "write() failed TcpClient: " << std::strerror(errno)
                      << "\n";
            return -1;
        }
        string_pos += current_bytes_send;
    }
    total_bytes_sent += string_pos;
    return total_bytes_sent;
}
auto TcpClient::read() -> std::string
{
    if (!connected) {
        return "";
    }
    char buffer[buffer_size];
    size_t bytes_to_read{};
    int current_bytes_read =
        ::read(fd, (char*)&bytes_to_read, sizeof(bytes_to_read));
    if (current_bytes_read < 0) {
        std::cerr << "read() failed TcpClient: " << std::strerror(errno)
                  << "\n";
        return "";
    }
    std::string result{};
    result.resize(bytes_to_read);
    size_t string_pos{};
    while (static_cast<int>(bytes_to_read) - static_cast<int>(buffer_size) > 0
    ) {
        int current_bytes_read =
            ::read(fd, result.data() + string_pos, buffer_size);
        if (current_bytes_read < 0) {
            std::cerr << "read() failed TcpClient: " << std::strerror(errno)
                      << "\n";
            return "";
        }
        string_pos += current_bytes_read;
    }
    int bytes_left = bytes_to_read - string_pos;
    if (bytes_left > 0) {
        int current_bytes_read =
            ::read(fd, result.data() + string_pos, bytes_left);
        if (current_bytes_read < 0) {
            std::cerr << "read() failed TcpClient: " << std::strerror(errno)
                      << "\n";
            return "";
        }
    }
    return result;
}
