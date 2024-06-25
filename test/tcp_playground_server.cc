#include "server.h"
#include <fstream>
#include <sys/wait.h>

int main()
{
    int port = 50011;
    size_t random_string_size{1'000'000'000};
    std::string random_string{};
    while (random_string.size() < random_string_size) {
        random_string.push_back(97 + (random() % 26));
    }
    std::ofstream file{};
    file.open("random.bin");
    file.write(random_string.data(), random_string.size());
    file.close();
    std::cout << "file written\n";

    TcpServer s1("127.0.0.1", port);
    std::cout << "connection accepted: " << s1.find_connection(5) << "\n";
    if (!s1.is_valid()) {
        std::exit(1);
    }
    std::cout << s1.read_connection() << "\n";
    s1.write_connection("hello client");
    s1.write_connection(random_string);
    return 0;
}
