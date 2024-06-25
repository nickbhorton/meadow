#include <fstream>
#include <iostream>
#include <memory>
#include <unistd.h>

#include "basic_http_parser.h"
#include "helpers.h"
#include "http_response.h"
#include "request_handles.h"

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

    RequestContext rc{};
    auto const [request_method, target, version] = start_line_opt.value();
    rc.version = version;
    rc.request_method = request_method;

    auto const [raw_url, query] = http_parser->parse_target(target);
    rc.url_filepath = std::move(raw_url);
    if (query) {
        rc.url_query = std::move(query.value());
    }

    std::vector<std::string> raw_headers{
        http_parser->split(connection.read_header())
    };
    for (auto& raw_header : raw_headers) {
        auto const header_opt =
            http_parser->parse_header(std::move(raw_header));
        if (header_opt.has_value()) {
            auto const& [header, info] = header_opt.value();
            if (header == "Content-Length") {
                try {
                    int content_length = stoi(info);
                    if (content_length > 0) {
                        rc.payload = connection.read(content_length);
                    }
                } catch (std::exception& e) {
                    std::cerr << "stoi() failed on Content-Length header\n";
                    rc.payload = "";
                }
            }
            rc.headers.push_back(std::move(header_opt.value()));
        }
    }

    switch (rc.request_method) {
    case http::RequestMethod::Get:
        handle_get_request(rc, context, connection);
        break;
    case http::RequestMethod::Post:
        handle_post_request(rc, context, connection);
        break;
    default:
        http::ResponseHeader response_header{501, "Not Implemented"};
        response_header.add_header("Connection", "closed");
        connection.write(response_header.to_string());
        break;
    }
}

void handle_get_request(
    RequestContext const& rc,
    ServerContext const& sc,
    http::Connection& connection
)
{
    std::string const location =
        server_locate(rc.url_filepath, sc.server_side_locations);
    std::string const filepath{sc.mount_point + location};
    std::cout << "GET: " << filepath << "  |  " << rc.url_filepath << "\n";
    std::ifstream file(filepath.c_str());
    if (file.good() && location.size() > 0) {
        http::ResponseHeader response_header{200, "Ok"};
        response_header.add_header(
            "Content-Type",
            extension_to_mime_type(
                get_file_extension(filepath),
                sc.extension_to_mime_type
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
}

void handle_post_request(
    RequestContext const& rc,
    ServerContext const& sc,
    http::Connection& connection
)
{
    std::cout << "POST to endpoint " << rc.url_filepath << "\n";
    for (auto const& [name, args] : rc.headers) {
        std::cout << "\t" << name << " : " << args << "\n";
    }
    if (rc.payload.size()) {
        std::cout << "Payload size: " << rc.payload.size() << "\n";
        sockaddr_in sendto_address{};
        bool found{false};
        for (auto const& [servername, address] : sc.endpoints) {
            if (servername == rc.url_filepath) {
                std::cout << "Port: " << address.sin_port << "\n";
                sendto_address = address;
                found = true;
            }
        }
        if (found) {
            int udp_fd{};
            if ((udp_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                perror("socket creation failed");
                exit(EXIT_FAILURE);
            }
            sockaddr_in servaddr{};
            servaddr.sin_family = AF_INET; // IPv4
            servaddr.sin_addr.s_addr = INADDR_ANY;
            // find a valid port
            servaddr.sin_port = 0;

            // Bind the socket with the server address
            if (bind(
                    udp_fd,
                    (const struct sockaddr*)&servaddr,
                    sizeof(servaddr)
                ) < 0) {
                perror("bind failed");
                exit(EXIT_FAILURE);
            }
            ssize_t sent_count = sendto(
                udp_fd,
                rc.payload.data(),
                rc.payload.size(),
                MSG_CONFIRM,
                (const struct sockaddr*)&sendto_address,
                sizeof(sendto_address)
            );
            std::cout << "Bytes sent: " << sent_count << "\n";
            sockaddr_in cliaddr{};
            socklen_t len = sizeof(cliaddr); // len is value/result
            size_t constexpr max_udp_packet_size{65536};
            char buffer[max_udp_packet_size];
            int recv_count = recvfrom(
                udp_fd,
                (char*)buffer,
                max_udp_packet_size,
                MSG_WAITALL,
                (struct sockaddr*)&cliaddr,
                &len
            );
            std::string recv_string(buffer, recv_count);
            std::cout << "From endpoint: " << recv_string << "\n";
            close(udp_fd);

            http::ResponseHeader response_header{200, "Ok"};
            response_header.add_header("Connection", "closed");
            connection.write(response_header.to_string());
            connection.write(recv_string);
        } else {
            http::ResponseHeader response_header{503, "Service Unavalable"};
            response_header.add_header("Connection", "closed");
            connection.write(response_header.to_string());
        }
    } else {
        http::ResponseHeader response_header{400, "Bad Request"};
        response_header.add_header("Connection", "closed");
        connection.write(response_header.to_string());
    }
}
