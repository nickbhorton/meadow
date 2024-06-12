#include "basic_http_parser.h"
#include "http_parser.h"
#include "http_response.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

struct ServerContext {
    std::string mount_point;
    std::vector<std::pair<std::string, std::string>> server_side_redirects;
};

void usage();

// helper functions i am to lazy to refactor
auto read_connection_line(int fd) -> std::string;
auto read_until_double_newline(int fd) -> std::string;

void handle_request(int connection_fd, ServerContext const& context)
{
    std::unique_ptr<http::Parser> const parser =
        std::make_unique<http::BasicParser>();
    auto const start_line_opt =
        parser->parse_request_line(read_connection_line(connection_fd));
    if (!start_line_opt.has_value()) {
        http::ResponseHeader response_header{};
        response_header.set_status_code(400);
        response_header.set_status_message("Bad Request");
        std::string response{response_header.to_string()};
        int n = write(connection_fd, response.c_str(), response.size());
        if (n < 0) {
            std::cerr << "failed to write response\n";
        }
        return;
    }

    auto const [request_method, target, version] = start_line_opt.value();
    auto const [url, query] = parser->parse_target(target);
    std::vector<std::pair<std::string, std::string>> headers{};
    {
        std::vector<std::string> raw_headers{
            parser->split(read_until_double_newline(connection_fd))
        };
        int failed_headers{0};
        for (auto& raw_header : raw_headers) {
            auto const header_opt = parser->parse_header(std::move(raw_header));
            if (header_opt.has_value()) {
                headers.push_back(std::move(header_opt.value()));
            } else {
                failed_headers++;
            }
        }
    }
    switch (request_method) {
    // GET
    case http::RequestMethod::Get: {
        std::string redirected_url{url};
        for (auto const& [in, out] : context.server_side_redirects) {
            if (redirected_url == in) {
                redirected_url = out;
            }
        }
        std::string filepath = context.mount_point + redirected_url;
        std::cout << "file in GET request: " << filepath << "\n";
        std::ifstream file(filepath.c_str());
        if (file.good()) {
            http::ResponseHeader response_header{};
            response_header.set_status_code(200);
            response_header.set_status_message("Ok");
            std::string response{response_header.to_string()};
            int n = write(connection_fd, response.c_str(), response.size());
            if (n < 0) {
                std::cerr << "failed to write response\n";
            }
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string contents = buffer.str();
            n = write(connection_fd, contents.c_str(), contents.size());
            if (n < 0) {
                std::cerr << "failed to write response\n";
            }
            return;
        } else {
            http::ResponseHeader response_header{};
            response_header.set_status_code(404);
            response_header.set_status_message("Not Found");
            std::string response{response_header.to_string()};
            int n = write(connection_fd, response.c_str(), response.size());
            if (n < 0) {
                std::cerr << "failed to write response\n";
            }
            return;
        }
    } break;

    default:
        http::ResponseHeader response_header{};
        response_header.set_status_code(501);
        response_header.set_status_message("Not Implemented");
        std::string response{response_header.to_string()};
        int n = write(connection_fd, response.c_str(), response.size());
        if (n < 0) {
            std::cerr << "failed to write response\n";
        }
        return;

        break;
    }
}

int main(int argc, char** argv)
{
    if (argc != 3) {
        usage();
        std::exit(1);
    }
    ServerContext const context = {
        .mount_point = argv[2],
        .server_side_redirects = {{"/", "/index.html"}}
    };
    std::cout << "server mounted on " << context.mount_point << "\n";
    sockaddr_in server_address{};
    server_address.sin_port = htons(atoi(argv[1]));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        std::cerr << "socket() call failed\n";
        std::exit(1);
    }

    if (bind(
            socket_fd,
            (struct sockaddr*)&server_address,
            sizeof(server_address)
        ) < 0) {
        std::cerr << "bind() call failed\n";
        std::exit(1);
    }

    while (true) {
        listen(socket_fd, 5);
        sockaddr_in client_address{};
        socklen_t client_length = sizeof(client_address);
        int connection_fd = accept(
            socket_fd,
            (struct sockaddr*)&client_address,
            &client_length
        );
        if (connection_fd < 0) {
            std::cerr << "accept() call failed\n";
            close(connection_fd);
        } else {
            handle_request(connection_fd, context);
            close(connection_fd);
        }
    }
}

void usage() { std::cout << "meadow [port] [mount dir]\n"; }

auto read_connection_line(int fd) -> std::string
{
    std::string line{};
    while (true) {
        char c;
        int n = read(fd, &c, 1);
        if (c == '\n') {
            break;
        } else {
            line.push_back(c);
        }
    }
    return line;
}
auto read_until_double_newline(int fd) -> std::string
{
    std::string line{};
    size_t constexpr buff_size = 4;
    char buffer[buff_size] = {0, 0, 0, 0};
    size_t ptr{0};
    while (true) {
        int n = read(fd, buffer + (ptr % 4), 1);
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
