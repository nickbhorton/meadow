cmake_minimum_required(VERSION 3.29)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED On)
add_compile_options(-g -Wall -Wextra -Wpedantic)

project(
    meadow
    LANGUAGES CXX
)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(INSTALL_GTEST OFF)
FetchContent_MakeAvailable(googletest)

add_library(
    http_parser
    src/basic_http_parser.cc
    src/http_response.cc
)

add_library(
    server_utils 
    src/server.cc
    src/client.cc
)

add_executable(
    meadow
    src/meadow.cc
    src/helpers.cc
    src/connection.cc
    src/request_handles.cc
)
target_link_libraries(
    meadow
    http_parser
    server_utils
)

add_executable(
    udp_echo 
    src/servers/udp_echo.cc
)
target_link_libraries(
    udp_echo
    server_utils 
)
target_include_directories(
    udp_echo
    PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

add_executable(
    login_server 
    src/servers/login_server.cc
)
target_link_libraries(
    login_server 
    server_utils 
)
target_include_directories(
    login_server 
    PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

install(
    TARGETS
    meadow
    udp_echo
    login_server
    RUNTIME
    DESTINATION
    /home/nick/.local/bin
)

# TESTS
add_executable(
  http_test
  test/http_parser_split.cc
  test/http_parser_parse_request_line.cc
  test/http_parser_parse_header.cc
  test/http_parser_parse_target.cc
)
target_link_libraries(
  http_test
  http_parser
  GTest::gtest_main
)
target_include_directories(
  http_test
  PUBLIC
  ${CMAKE_SOURCE_DIR}/src
)

add_executable(
    tcp_playground_server 
    test/tcp_playground_server.cc
)
target_link_libraries(
    tcp_playground_server 
    server_utils
)
target_include_directories(
    tcp_playground_server 
    PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

add_executable(
    tcp_playground_client
    test/tcp_playground_client.cc
)
target_link_libraries(
    tcp_playground_client
    server_utils
)
target_include_directories(
    tcp_playground_client
    PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

enable_testing()
include(GoogleTest)
gtest_discover_tests(http_test)

