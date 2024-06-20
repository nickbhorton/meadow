#include "basic_http_parser.h"
#include "connection.h"
#include "helpers.h"
#include "http_parser.h"
#include "http_response.h"
#include <fstream>
#include <iostream>
#include <memory>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

struct ServerContext {
    int port;
    std::string mount_point;
    std::vector<std::pair<std::string, std::string>> server_side_locations;
    std::vector<std::pair<std::string, std::string>> extension_to_mime_type;
};

std::unique_ptr<http::Parser> const http_parser =
    std::make_unique<http::BasicParser>();

void handle_request(http::Connection& connection, ServerContext const& context)
{
    auto const start_line_opt =
        http_parser->parse_request_line(connection.read_line());

    if (!start_line_opt.has_value()) {
        http::ResponseHeader response_header{400, "Bad Request"};
        response_header.add_header("Connection", "closed");
        connection.write(response_header.to_string());
        return;
    }

    auto const [request_method, target, version] = start_line_opt.value();
    auto const [raw_url, query] = http_parser->parse_target(target);
    std::vector<std::pair<std::string, std::string>> headers{};
    {
        std::vector<std::string> raw_headers{
            http_parser->split(connection.read_header())
        };
        for (auto& raw_header : raw_headers) {
            auto const header_opt =
                http_parser->parse_header(std::move(raw_header));
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
        std::cout << "GET: " << filepath << "|" << raw_url << "\n";
        std::ifstream file(filepath.c_str());
        if (file.good() && location.size() > 0) {
            http::ResponseHeader response_header{200, "Ok"};
            response_header.add_header(
                "Content-Type",
                extension_to_mime_type(
                    get_file_extension(filepath),
                    context.extension_to_mime_type
                )
            );
            response_header.add_header("Connection", "closed");
            connection.write(response_header.to_string());
            connection.write(read_file_to_string(file));
        } else {
            std::cout << "file not found\n";
            http::ResponseHeader response_header{404, "Not Found"};
            response_header.add_header("Connection", "closed");
            connection.write(response_header.to_string());
        }
    } break;

    default:
        std::cout << request_method << ": " << raw_url << "\n";
        std::string payload{};
        for (auto const& [first, second] : headers) {
            std::cout << "\t" << first << ": " << second << "\n";
            if (first == "Content-Length") {
                payload = connection.read(stoi(second));
            }
        }
        if (payload.size()) {
            std::cout << "Payload" << payload << "\n";
        }
        http::ResponseHeader response_header{501, "Not Implemented"};
        response_header.add_header("Connection", "closed");
        connection.write(response_header.to_string());
        break;
    }
}

int main(int argc, char** argv)
{
    std::ifstream meadow_file{};
    meadow_file.open("meadow.txt");
    if (!meadow_file.good()) {
        std::cerr << "did not find meadow.txt file\n";
        std::exit(1);
    }
    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    ServerContext context{};
    std::string line{};
    while (!meadow_file.eof()) {
        std::getline(meadow_file, line);
        std::stringstream ss{line};
        std::string token{};
        ss >> token;
        if (token == "mount") {
            token.clear();
            ss >> token;
            context.mount_point = token;
        } else if (token == "port") {
            token.clear();
            ss >> token;
            context.port = stoi(token);
            server_address.sin_port = htons(context.port);
        } else if (token == "index") {
            token.clear();
            ss >> token;
            std::cout << "index: " << token << "\n";
            context.server_side_locations.push_back({"/", token});
        } else if (token == "ext") {
            std::string mime_type{};
            std::string ext_type{};
            ss >> ext_type;
            ss >> mime_type;
            context.extension_to_mime_type.push_back({ext_type, mime_type});
        } else if (token == "endpoint") {
            std::string method{};
            std::string pointname{};
            std::string filename{};
            ss >> method;
            ss >> pointname;
            ss >> filename;
            std::cout << pointname << " " << filename << "\n";
            context.server_side_locations.push_back({pointname, filename});
        } else if (token.size() > 0) {
            std::cout << "unrecognized token: " << token << "\n";
        }
    }
    std::cout << "Meadow server mounted on " << context.mount_point << "\n";
    std::cout << "Port: " << context.port << "\n";

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
        int enable{1};
        if (setsockopt(
                socket_fd,
                SOL_SOCKET,
                SO_REUSEADDR,
                &enable,
                sizeof(enable)
            ) < 0) {
            std::cerr << "setsockopt() SO_REUSEADDR call failed\n";
            std::exit(1);
        }
        if (setsockopt(
                socket_fd,
                SOL_SOCKET,
                SO_REUSEPORT,
                &enable,
                sizeof(enable)
            ) < 0) {
            std::cerr << "setsockopt() SO_REUSEPORT call failed\n";
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
            http::Connection connection(connection_fd);
            handle_request(connection, context);
        }
    }
}
