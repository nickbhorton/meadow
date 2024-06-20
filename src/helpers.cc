#include "helpers.h"

#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unistd.h>

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
