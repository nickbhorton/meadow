#include "client.h"
#include "server.h"

int main()
{
    pid_t id = fork();
    int port = 50011;
    if (id) {
        TcpServer s1("127.0.0.1", port);
        std::cout << "connection accepted: " << s1.find_connection(5) << "\n";
        std::cout << "server read: " << s1.read_connection() << "\n";
        std::cout << "server wrote bytes: "
                  << s1.write_connection("hello client") << "\n";
    } else {
        TcpClient c1{};
        std::cout << "client connected: " << c1.connect("127.0.0.1", port)
                  << "\n";
        std::cout << "client wrote bytes: " << c1.write("test") << "\n";
        std::cout << "client read: " << c1.read() << "\n";
    }
}
