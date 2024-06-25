#pragma once

#include <netinet/in.h>
#include <string>
#include <vector>

#include "connection.h"
#include "http_parser.h"

struct ServerContext {
    int port;
    std::string mount_point;
    std::string data_path;
    std::vector<std::pair<std::string, std::string>> server_side_locations;
    std::vector<std::tuple<std::string, http::RequestMethod, sockaddr_in>>
        endpoints;
    std::vector<std::pair<std::string, std::string>> extension_to_mime_type;
};

struct RequestContext {
    http::RequestMethod request_method;
    http::ProtocolVersion version;
    std::string url_filepath;
    std::string url_query;
    std::vector<std::pair<std::string, std::string>> headers;
    std::string payload;
};

void handle_request(http::Connection& connection, ServerContext const& context);

void handle_get_request(
    RequestContext const& rc,
    ServerContext const& sc,
    http::Connection& connection
);

void handle_post_request(
    RequestContext const& rc,
    ServerContext const& sc,
    http::Connection& connection
);
