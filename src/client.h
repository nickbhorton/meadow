#pragma once
#include <netinet/in.h>
#include <string>

class TcpClient
{
    int fd;
    bool connected;

public:
    TcpClient();
    ~TcpClient();
    TcpClient& operator=(TcpClient const&) = delete;
    TcpClient& operator=(TcpClient&&) = delete;
    TcpClient(TcpClient const&) = delete;
    TcpClient(TcpClient&&) = delete;

    auto connect(std::string const& address, int port) -> bool;
    auto write(std::string const& mesg) -> int;
    auto read() -> std::string;
};
