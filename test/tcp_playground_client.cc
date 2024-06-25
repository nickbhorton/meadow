#include "client.h"
#include <fstream>
#include <iostream>

int main()
{
    int port = 50011;
    TcpClient c1{};
    std::cout << "client connected: " << c1.connect("127.0.0.1", port) << "\n";
    c1.write_serialized_string("hello server");
    std::string first_read{c1.read_serialized_string()};
    std::string random_string_copy{c1.read_serialized_string()};
    std::ifstream file{};
    char c{};
    std::string random_string{};
    file.open("random.bin");
    while (file >> c) {
        random_string.push_back(c);
    }
    file.close();
    // std::cout << random_string_copy << "\n";
    std::cout << "random string send correctly: "
              << (random_string_copy == random_string) << "\n";
    std::exit(0);
}
