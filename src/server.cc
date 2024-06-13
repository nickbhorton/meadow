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
    std::vector<std::pair<std::string, std::string>> server_side_locations;
    std::vector<std::pair<std::string, std::string>> extension_to_mime_type;
};

void usage();

// helper functions i am to lazy to refactor
auto read_file_to_string(std::ifstream& file) -> std::string;
auto read_connection_line(int fd) -> std::string;
auto read_until_double_newline(int fd) -> std::string;
auto write_connection(int fd, std::string const& msg) -> int;
auto get_file_extension(std::string const& filepath) -> std::string;

auto server_locate(
    std::string const& raw_url,
    std::vector<std::pair<std::string, std::string>> const& locations
) -> std::string;
auto extension_to_mime_type(
    std::string const& extension,
    std::vector<std::pair<std::string, std::string>> const&
        extension_to_mime_type
) -> std::string;

void handle_request(int connection_fd, ServerContext const& context)
{
    std::unique_ptr<http::Parser> const parser =
        std::make_unique<http::BasicParser>();

    auto const start_line_opt =
        parser->parse_request_line(read_connection_line(connection_fd));

    if (!start_line_opt.has_value()) {
        http::ResponseHeader response_header{400, "Bad Request"};
        write_connection(connection_fd, response_header.to_string());
        return;
    }

    auto const [request_method, target, version] = start_line_opt.value();
    auto const [raw_url, query] = parser->parse_target(target);
    std::vector<std::pair<std::string, std::string>> headers{};
    {
        std::vector<std::string> raw_headers{
            parser->split(read_until_double_newline(connection_fd))
        };
        for (auto& raw_header : raw_headers) {
            auto const header_opt = parser->parse_header(std::move(raw_header));
            if (header_opt.has_value()) {
                headers.push_back(std::move(header_opt.value()));
            }
        }
    }

    switch (request_method) {
    case http::RequestMethod::Get: {
        std::string location =
            server_locate(raw_url, context.server_side_locations);
        std::string const filepath{context.mount_point + location};
        std::ifstream file(filepath.c_str());
        if (file.good() && location.size() > 0) {
            std::cout << "GET: " << filepath << "\n";
            http::ResponseHeader response_header{200, "Ok"};
            response_header.add_header(
                "Content-Type",
                extension_to_mime_type(
                    get_file_extension(filepath),
                    context.extension_to_mime_type
                )
            );
            write_connection(connection_fd, response_header.to_string());
            write_connection(connection_fd, read_file_to_string(file));
        } else {
            http::ResponseHeader response_header{404, "Not Found"};
            write_connection(connection_fd, response_header.to_string());
        }
    } break;

    default:
        http::ResponseHeader response_header{501, "Not Implemented"};
        write_connection(connection_fd, response_header.to_string());
        break;
    }
}

int main(int argc, char** argv)
{
    if (argc != 3) {
        usage();
        std::exit(1);
    }
    ServerContext const context =
        {.mount_point = argv[2],
         .server_side_locations =
             {{"/", "/index.html"},
              {"/styles.css", "/styles.css"},
              {"/me.jpg", "/me.jpg"},
              {"/", "/index.html"}},
         .extension_to_mime_type = {
             {"html", "text/html"},
             {"css", "text/css"},
             {"js", "text/javascript"},
             {"txt", "text/plain"},
             {"jpg", "image/jpeg"},
             {"jpeg", "image/jpeg"},
             {"png", "image/png"},
         }};
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
    {
        int x{1};
        if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &x, sizeof(x)) <
            0) {
            std::cerr << "setsockopt() call failed\n";
            std::exit(1);
        }
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

auto write_connection(int fd, std::string const& msg) -> int
{
    int n = write(fd, msg.c_str(), msg.size());
    if (n < 0) {
        std::cerr << "failed to write response in write connection\n";
    }
    return n;
}

auto read_file_to_string(std::ifstream& file) -> std::string
{
    std::stringstream buffer{};
    buffer << file.rdbuf();
    return buffer.str();
}

auto server_locate(
    std::string const& raw_url,
    std::vector<std::pair<std::string, std::string>> const& redirects
) -> std::string
{
    std::string result{};
    for (auto const& [in, out] : redirects) {
        if (raw_url == in) {
            result = out;
        }
    }
    return result;
}

auto get_file_extension(std::string const& filepath) -> std::string
{
    auto const dot_pos{filepath.find(".")};
    if (dot_pos != std::string::npos) {
        std::string extention{filepath.substr(dot_pos + 1)};
        return extention;
    } else {
        std::cerr << "file extension not found for " << filepath << "\n";
        return "txt";
    }
}

auto extension_to_mime_type(
    std::string const& extension,
    std::vector<std::pair<std::string, std::string>> const&
        extension_to_mime_type
) -> std::string
{
    for (auto const& [test_extension, mime_type] : extension_to_mime_type) {
        if (test_extension == extension) {
            return mime_type;
        }
    }
    std::cerr << "mime type not found for " << extension << "\n";
    return "text/plain";
}
