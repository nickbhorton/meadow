#include <arpa/inet.h>
#include <fstream>
#include <iostream>
#include <netinet/in.h>
#include <pthread.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

#include "connection.h"
#include "request_handles.h"
#include "server.h"

int main()
{
    std::ifstream meadow_file{};
    meadow_file.open("meadow.txt");
    if (!meadow_file.good()) {
        std::cerr << "did not find meadow.txt file\n";
        std::exit(1);
    }
    ServerContext context{};
    std::string line_buffer{};
    while (!meadow_file.eof()) {
        std::getline(meadow_file, line_buffer);
        std::stringstream ss{line_buffer};
        std::string word{};
        ss >> word;
        if (word == "mount") {
            word.clear();
            ss >> word;
            context.mount_point = word;
        } else if (word == "port") {
            word.clear();
            ss >> word;
            context.port = stoi(word);
        } else if (word == "data") {
            word.clear();
            ss >> word;
            context.data_path = word;
        } else if (word == "index") {
            word.clear();
            ss >> word;
            std::cout << "index: " << word << "\n";
            context.server_side_locations.push_back({"/", word});
        } else if (word == "ext") {
            std::string mime_type{};
            std::string ext_type{};
            ss >> ext_type;
            ss >> mime_type;
            context.extension_to_mime_type.push_back({ext_type, mime_type});
        } else if (word == "location") {
            std::string method{};
            std::string pointname{};
            std::string filename{};
            ss >> method;
            ss >> pointname;
            ss >> filename;
            std::cout << "location: " << pointname << " " << filename << "\n";
            context.server_side_locations.push_back({pointname, filename});

        } else if (word == "endpoint") {
            std::string servername{};
            std::string address{};
            int port{};
            ss >> servername;
            ss >> address;
            ss >> port;
            sockaddr_in addr{};
            addr.sin_family = AF_INET;
            inet_pton(AF_INET, address.c_str(), &addr.sin_addr);
            addr.sin_port = htons(port);
            std::cout << "endpoint: " << servername << " " << address << ":"
                      << port << "\n";
            context.endpoints.push_back({servername, addr});
        } else if (word.size() > 0) {
            std::cout << "unrecognized token: " << word << "\n";
        }
    }
    std::cout << "Meadow server mounted on " << context.mount_point << "\n";
    std::cout << "Port: " << context.port << "\n";

    TcpServer server{"127.0.0.1", context.port};
    while (true) {
        server.find_connection(5);
        int connection_fd = server.get_connection_fd();
        if (connection_fd < 0) {
            std::cerr << "TcpServer::get_connection_fd() call failed\n";
        } else {
            http::Connection connection(connection_fd);
            handle_request(connection, context);
        }
    }
}
