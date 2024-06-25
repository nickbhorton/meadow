#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

auto write_string(int fd, std::string const& mesg, bool serialize = true)
    -> int;
auto read_serialized_string(int fd) -> std::string;

class UdpServer
{
    sockaddr_in address;
    bool valid;
    int fd;

public:
    UdpServer(
        std::string server_address,
        int port,
        bool reuse_address = true,
        bool reuse_port = true
    );
    ~UdpServer();
    UdpServer& operator=(UdpServer const&) = delete;
    UdpServer& operator=(UdpServer&&) = delete;
    UdpServer(UdpServer const&) = delete;
    UdpServer(UdpServer&&) = delete;

    auto get_port() const -> int;

    auto send(std::string const& msg, sockaddr_in const& to_address) -> int;
    auto recv() -> std::optional<std::tuple<std::string, sockaddr_in>>;
};

class TcpServer
{
    sockaddr_in address;
    int fd;
    bool valid;

    sockaddr_in connection_address;
    int connection_fd;
    bool connection_active;

public:
    TcpServer(
        std::string server_address,
        int port,
        bool reuse_address = true,
        bool reuse_port = true
    );
    ~TcpServer();
    TcpServer& operator=(TcpServer const&) = delete;
    TcpServer& operator=(TcpServer&&) = delete;
    TcpServer(TcpServer const&) = delete;
    TcpServer(TcpServer&&) = delete;

    auto get_connection_fd() -> int;
    auto get_port() const -> int;
    auto is_valid() const -> bool;
    auto find_connection(int backlog) -> bool;
    auto close_connection() -> bool;
    auto write_serialized_string_connection(std::string const& mesg) -> int;
    auto write_string_connection(std::string const& mesg) -> int;
    auto read_serialized_string_connection() -> std::string;
};
