#include "meadow_server.h"

int main(int argc, char** argv)
{
    if (argc != 3) {
        std::cout << "udp_echo [address] [port]\n";
    }
    UdpServer server(argv[1], atoi(argv[2]));
    while (true) {
        auto const response_opt = server.recv();
        if (!response_opt.has_value()) {
            continue;
        } else {
            auto const& [str, addr] = response_opt.value();
            int send_count = server.send(str, addr);
            std::cout << "sent: " << send_count << "\n";
        }
    }
    return 0;
}
