#include "server.h"
#include <unistd.h>

constexpr size_t buffer_size = 1024;

auto write_string(int fd, std::string const& mesg, bool serialize) -> int
{
    unsigned int string_size = mesg.size();
    int total_bytes_sent{0};
    if (serialize) {
        int current_bytes_sent =
            ::write(fd, (const char*)&string_size, sizeof(string_size));
        if (current_bytes_sent < 0) {
            std::cerr << "write() failed TcpServer: " << std::strerror(errno)
                      << "\n";
            return -1;
        }
        // std::cout << "bytes sent count: " << current_bytes_sent << "\n";
        // std::cout << "write_string size: " << string_size << "\n";
        total_bytes_sent += current_bytes_sent;
    }

    unsigned int string_pos{0};
    while (string_pos + buffer_size < mesg.size()) {
        int current_bytes_send =
            ::write(fd, mesg.data() + string_pos, sizeof(buffer_size));
        if (current_bytes_send < 0) {
            std::cerr << "write() failed TcpServer: " << std::strerror(errno)
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
            std::cerr << "write() failed TcpServer: " << std::strerror(errno)
                      << "\n";
            return -1;
        }
        string_pos += current_bytes_send;
    }
    total_bytes_sent += string_pos;
    return total_bytes_sent;
}

auto read_serialized_string(int fd) -> std::string
{
    // std::cout << "START READ\n";
    unsigned int bytes_to_read{0};
    int current_bytes_read =
        ::read(fd, (char*)&bytes_to_read, sizeof(bytes_to_read));
    // std::cout << "bytes recv count: " << current_bytes_read << "\n";
    // std::cout << "read_string size: " << bytes_to_read << "\n";
    if (current_bytes_read < 0) {
        std::cerr << "read() failed TcpServer: " << std::strerror(errno)
                  << "\n";
        return "";
    }
    std::string result{};
    result.resize(bytes_to_read);
    unsigned int string_pos{0};
    while (static_cast<int>(bytes_to_read) - static_cast<int>(string_pos) > 0) {
        int current_bytes_read = ::read(
            fd,
            result.data() + string_pos,
            (buffer_size > bytes_to_read) ? bytes_to_read : buffer_size
        );
        if (current_bytes_read < 0) {
            std::cerr << "read() failed TcpServer: " << std::strerror(errno)
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
            std::cerr << "read() failed TcpServer: " << std::strerror(errno)
                      << "\n";
            return "";
        }
    }
    // std::cout << "STRING READ RESULT: " << result << "\n";
    // std::cout << result.size() << "\n";
    return result;
}

UdpServer::UdpServer(
    std::string server_address,
    int port,
    bool reuse_address,
    bool reuse_port
)
    : address{}, valid{false}
{
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "socket() failed UdpServer\n";
        return;
    }
    // Filling server information
    address.sin_family = AF_INET; // IPv4
    address.sin_addr.s_addr = INADDR_ANY;
    inet_pton(address.sin_family, server_address.c_str(), &address.sin_addr);
    address.sin_port = htons(port);

    int enable{1};
    if (reuse_address) {
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) <
            0) {
            std::cerr << "setsockopt() SO_REUSEADDR call failed\n";
            std::exit(1);
        }
    }
    if (reuse_port) {
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(enable)) <
            0) {
            std::cerr << "setsockopt() SO_REUSEPORT call failed\n";
            std::exit(1);
        }
    }

    if (bind(fd, (const struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "bind() failed UdpServer\n";
        return;
    }
    socklen_t socket_length{sizeof(address)};
    if (getsockname(fd, (struct sockaddr*)&address, &socket_length) < 0) {
        std::cerr << "getsockname() failed UdpServer\n";
        return;
    }
    valid = true;
}

UdpServer::~UdpServer() { close(fd); }

auto UdpServer::send(std::string const& msg, sockaddr_in const& to_address)
    -> int
{
    if (!valid) {
        return -1;
    }
    ssize_t sent_count = sendto(
        fd,
        msg.data(),
        msg.size(),
        MSG_CONFIRM,
        (const struct sockaddr*)&to_address,
        sizeof(to_address)
    );
    return sent_count;
}
auto UdpServer::recv() -> std::optional<std::tuple<std::string, sockaddr_in>>
{
    if (!valid) {
        return {};
    }
    sockaddr_in client_address{};
    socklen_t client_address_length =
        sizeof(client_address); // len is value/result

    size_t constexpr max_udp_packet_size{65536};
    char buffer[max_udp_packet_size];
    int n = recvfrom(
        fd,
        (char*)buffer,
        max_udp_packet_size,
        MSG_WAITALL,
        (struct sockaddr*)&client_address,
        &client_address_length
    );
    std::string payload(buffer, n);
    return {{payload, client_address}};
}

auto UdpServer::get_port() const -> int { return ntohs(address.sin_port); }

TcpServer::TcpServer(
    std::string server_address,
    int port,
    bool reuse_address,
    bool reuse_port
)
{
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "socket() failed TcpServer: " << std::strerror(errno)
                  << "\n";
        return;
    }
    // Filling server information
    address.sin_family = AF_INET; // IPv4
    address.sin_addr.s_addr = INADDR_ANY;
    inet_pton(address.sin_family, server_address.c_str(), &address.sin_addr);
    address.sin_port = htons(port);

    int enable{1};
    if (reuse_address) {
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) <
            0) {
            std::cerr << "getsockopt() failed TcpServer: "
                      << std::strerror(errno) << "\n";
            std::exit(1);
        }
    }
    if (reuse_port) {
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(enable)) <
            0) {
            std::cerr << "getsockopt() failed TcpServer: "
                      << std::strerror(errno) << "\n";
            std::exit(1);
        }
    }

    if (bind(fd, (const struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "bind() failed TcpServer\n";
        return;
    }
    socklen_t socket_length{sizeof(address)};
    if (getsockname(fd, (struct sockaddr*)&address, &socket_length) < 0) {
        std::cerr << "getsockname() failed TcpServer: " << std::strerror(errno)
                  << "\n";
        return;
    }
    valid = true;
}
TcpServer::~TcpServer() { close(fd); }

auto TcpServer::get_port() const -> int { return ntohs(address.sin_port); }
auto TcpServer::is_valid() const -> bool { return valid; };
auto TcpServer::find_connection(int backlog) -> bool
{
    int result = ::listen(fd, backlog);
    if (result < 0) {
        std::cerr << "listen() failed TcpServer: " << std::strerror(errno)
                  << "\n";
        connection_active = false;
    } else {
        socklen_t connection_address_length{sizeof(connection_address)};
        connection_fd = accept(
            fd,
            (struct sockaddr*)&connection_address,
            &connection_address_length
        );
        if (connection_fd < 0) {
            std::cerr << "accept() failed TcpServer: " << std::strerror(errno)
                      << "\n";
            connection_active = false;
        } else {
            connection_active = true;
        }
    }
    return connection_active;
}

auto TcpServer::close_connection() -> bool
{
    if (connection_active && connection_fd) {
        close(connection_fd);
        connection_fd = -1;
        connection_active = false;
        return true;
    }
    return false;
}

auto TcpServer::write_serialized_string_connection(std::string const& mesg)
    -> int
{
    if ((!connection_active) || (!connection_fd)) {
        return -1;
    }
    return write_string(connection_fd, mesg, true);
}
auto TcpServer::write_string_connection(std::string const& mesg) -> int
{
    if ((!connection_active) || (!connection_fd)) {
        return -1;
    }
    return write_string(connection_fd, mesg, false);
}

auto TcpServer::read_serialized_string_connection() -> std::string
{
    if ((!connection_active) || (!connection_fd)) {
        return "";
    }
    return read_serialized_string(connection_fd);
}

auto TcpServer::get_connection_fd() -> int
{
    if ((!connection_active) || (!connection_fd)) {
        return -1;
    }
    return connection_fd;
}
