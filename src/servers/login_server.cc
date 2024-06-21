#include "udp_server.h"
#include <sstream>
#include <tuple>

auto split_on(std::string str, char c) -> std::tuple<std::string, std::string>
{
    auto split_pos = str.find(c);
    std::string field1{str.substr(0, split_pos)};
    std::string field2{str.substr(split_pos + 1)};
    return {field1, field2};
}

int main(int argc, char** argv)
{
    std::ifstream meadow_file{};
    meadow_file.open("users.txt");
    if (!meadow_file.good()) {
        std::cerr << "did not find users.txt file\n";
        std::exit(1);
    }
    std::vector<std::tuple<std::string, std::string>> users{};
    std::string line_buffer{};
    while (!meadow_file.eof()) {
        std::getline(meadow_file, line_buffer);
        std::stringstream ss{line_buffer};
        std::string username{};
        std::string password{};
        ss >> username;
        ss >> password;
        if (username.size() && password.size()) {
            users.push_back({username, password});
            std::cout << username << " " << password << "\n";
        }
    }
    UdpServer server("127.0.0.1", 50000);
    while (true) {
        auto const response_opt = server.recv();
        if (!response_opt.has_value()) {
            continue;
        } else {
            auto const& [str, addr] = response_opt.value();
            std::cout << "REQUEST: " << str << "\n";
            auto [username_field, password_field] =
                split_on(std::move(str), '&');
            auto const [fieldname1, username_given] =
                split_on(std::move(username_field), '=');
            auto const [fieldname2, password_given] =
                split_on(std::move(password_field), '=');
            std::string response{"0"};
            for (auto const& [username, password] : users) {
                if (username_given == username && password_given == password) {
                    response = "1";
                }
                if (username_given == username && password_given != password) {
                    response = "2";
                }
            }
            int send_count = server.send(response, addr);
            std::cout << "RESPONSE: " << response << "\t" << send_count << "\n";
        }
    }
    return 0;
}
