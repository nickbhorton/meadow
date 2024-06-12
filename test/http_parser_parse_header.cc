#include <memory>

#include <gtest/gtest.h>

#include "basic_http_parser.h"
#include "http_parser.h"

TEST(ParserTest, ParseHeaderHappy)
{
    std::unique_ptr<http::Parser> parser =
        std::make_unique<http::BasicParser>();
    {
        std::string test_string{"Host: localhost:8000"};
        auto const result_opt = parser->parse_header(std::move(test_string));
        ASSERT_TRUE(result_opt.has_value());
        auto const& [first, second] = result_opt.value();
        EXPECT_EQ(first, "Host");
        EXPECT_EQ(second, " localhost:8000");
    }
}

TEST(ParserTest, ParseHeaderHappyComplex)
{
    std::unique_ptr<http::Parser> parser =
        std::make_unique<http::BasicParser>();
    {
        std::string test_string{
            "User-Agent: Mozilla/5.0 (Macintosh;... )... Firefox/51.0"
        };
        auto const result_opt = parser->parse_header(std::move(test_string));
        ASSERT_TRUE(result_opt.has_value());
        auto const& [first, second] = result_opt.value();
        EXPECT_EQ(first, "User-Agent");
        EXPECT_EQ(second, " Mozilla/5.0 (Macintosh;... )... Firefox/51.0");
    }
}

TEST(ParserTest, ParseHeaderNoColon)
{
    std::unique_ptr<http::Parser> parser =
        std::make_unique<http::BasicParser>();
    {
        std::string test_string{
            "User-Agent Mozilla/5.0 (Macintosh;... )... Firefox/51.0"
        };
        auto const result_opt = parser->parse_header(std::move(test_string));
        ASSERT_FALSE(result_opt.has_value());
    }
}
