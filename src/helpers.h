#pragma once

#include <string>
#include <vector>

auto read_file_to_string(std::ifstream& file) -> std::string;
auto read_connection_line(int fd) -> std::string;
auto read_until_double_newline(int fd) -> std::string;
auto write_connection(int fd, std::string const& msg) -> int;
auto get_file_extension(std::string const& filepath) -> std::string;

auto server_locate(
    std::string const& raw_url,
    std::vector<std::pair<std::string, std::string>> const& locations
) -> std::string;
auto extension_to_mime_type(
    std::string const& extension,
    std::vector<std::pair<std::string, std::string>> const&
        extension_to_mime_type
) -> std::string;
