#include "client.h"
#include "server.h"

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <unistd.h>


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
    return write_string(fd, mesg);
}

auto TcpClient::read() -> std::string
{
    if (!connected) {
        return "";
    }
    return read_string(fd);
}
