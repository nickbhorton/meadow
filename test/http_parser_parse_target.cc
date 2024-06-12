#include <memory>

#include <gtest/gtest.h>

#include "basic_http_parser.h"
#include "http_parser.h"

TEST(ParserTest, ParseTargetHappy)
{
    std::unique_ptr<http::Parser> parser =
        std::make_unique<http::BasicParser>();
    {
        std::string test_string{"/index.html"};
        auto const result_opt = parser->parse_target(std::move(test_string));
        auto const& [first, second] = result_opt;
        EXPECT_EQ(first, "/index.html");
        EXPECT_FALSE(second.has_value());
    }
}

TEST(ParserTest, ParseTargetHappyWithQuery)
{
    std::unique_ptr<http::Parser> parser =
        std::make_unique<http::BasicParser>();
    {
        std::string test_string{"/index.html?query=hello"};
        auto const result_opt = parser->parse_target(std::move(test_string));
        auto const& [first, second] = result_opt;
        EXPECT_EQ(first, "/index.html");
        ASSERT_TRUE(second.has_value());
        EXPECT_EQ(second.value(), "query=hello");
    }
}
