#include <memory>

#include <gtest/gtest.h>

#include "basic_http_parser.h"
#include "http_parser.h"

TEST(ParserTest, ParseRequestLineHappyGET)
{
    std::unique_ptr<http::Parser> parser =
        std::make_unique<http::BasicParser>();
    {
        std::string test_string{"GET / HTTP/1.1"};
        auto const result_opt =
            parser->parse_request_line(std::move(test_string));
        ASSERT_TRUE(result_opt.has_value());
        auto const& [request_method, target, version] = result_opt.value();
        EXPECT_EQ(request_method, http::RequestMethod::Get);
        EXPECT_EQ(target, "/");
        EXPECT_EQ(version, http::ProtocolVersion::OnePointOne);
    }
}

TEST(ParserTest, ParseRequestLineHappyVersion1)
{
    std::unique_ptr<http::Parser> parser =
        std::make_unique<http::BasicParser>();
    {
        std::string test_string{"GET / HTTP/1.0"};
        auto const result_opt =
            parser->parse_request_line(std::move(test_string));
        ASSERT_TRUE(result_opt.has_value());
        auto const& [request_method, target, version] = result_opt.value();
        EXPECT_EQ(request_method, http::RequestMethod::Get);
        EXPECT_EQ(target, "/");
        EXPECT_EQ(version, http::ProtocolVersion::One);
    }
}

TEST(ParserTest, ParseRequestLineHappyHEAD)
{
    std::unique_ptr<http::Parser> parser =
        std::make_unique<http::BasicParser>();
    {
        std::string test_string{"HEAD / HTTP/1.1"};
        auto const result_opt =
            parser->parse_request_line(std::move(test_string));
        ASSERT_TRUE(result_opt.has_value());
        auto const& [request_method, target, version] = result_opt.value();
        EXPECT_EQ(request_method, http::RequestMethod::Head);
        EXPECT_EQ(target, "/");
        EXPECT_EQ(version, http::ProtocolVersion::OnePointOne);
    }
}

TEST(ParserTest, ParseRequestLineHappyPOST)
{
    std::unique_ptr<http::Parser> parser =
        std::make_unique<http::BasicParser>();
    {
        std::string test_string{"POST / HTTP/1.1"};
        auto const result_opt =
            parser->parse_request_line(std::move(test_string));
        ASSERT_TRUE(result_opt.has_value());
        auto const& [request_method, target, version] = result_opt.value();
        EXPECT_EQ(request_method, http::RequestMethod::Post);
        EXPECT_EQ(target, "/");
        EXPECT_EQ(version, http::ProtocolVersion::OnePointOne);
    }
}

TEST(ParserTest, ParseRequestLineHappyPUT)
{
    std::unique_ptr<http::Parser> parser =
        std::make_unique<http::BasicParser>();
    {
        std::string test_string{"PUT / HTTP/1.1"};
        auto const result_opt =
            parser->parse_request_line(std::move(test_string));
        ASSERT_TRUE(result_opt.has_value());
        auto const& [request_method, target, version] = result_opt.value();
        EXPECT_EQ(request_method, http::RequestMethod::Put);
        EXPECT_EQ(target, "/");
        EXPECT_EQ(version, http::ProtocolVersion::OnePointOne);
    }
}

TEST(ParserTest, ParseRequestLineHappyDELETE)
{
    std::unique_ptr<http::Parser> parser =
        std::make_unique<http::BasicParser>();
    {
        std::string test_string{"DELETE / HTTP/1.1"};
        auto const result_opt =
            parser->parse_request_line(std::move(test_string));
        ASSERT_TRUE(result_opt.has_value());
        auto const& [request_method, target, version] = result_opt.value();
        EXPECT_EQ(request_method, http::RequestMethod::Delete);
        EXPECT_EQ(target, "/");
        EXPECT_EQ(version, http::ProtocolVersion::OnePointOne);
    }
}

TEST(ParserTest, ParseRequestLineHappyCONNECT)
{
    std::unique_ptr<http::Parser> parser =
        std::make_unique<http::BasicParser>();
    {
        std::string test_string{"CONNECT / HTTP/1.1"};
        auto const result_opt =
            parser->parse_request_line(std::move(test_string));
        ASSERT_TRUE(result_opt.has_value());
        auto const& [request_method, target, version] = result_opt.value();
        EXPECT_EQ(request_method, http::RequestMethod::Connect);
        EXPECT_EQ(target, "/");
        EXPECT_EQ(version, http::ProtocolVersion::OnePointOne);
    }
}

TEST(ParserTest, ParseRequestLineHappyOPTIONS)
{
    std::unique_ptr<http::Parser> parser =
        std::make_unique<http::BasicParser>();
    {
        std::string test_string{"OPTIONS / HTTP/1.1"};
        auto const result_opt =
            parser->parse_request_line(std::move(test_string));
        ASSERT_TRUE(result_opt.has_value());
        auto const& [request_method, target, version] = result_opt.value();
        EXPECT_EQ(request_method, http::RequestMethod::Options);
        EXPECT_EQ(target, "/");
        EXPECT_EQ(version, http::ProtocolVersion::OnePointOne);
    }
}

TEST(ParserTest, ParseRequestLineHappyTRACE)
{
    std::unique_ptr<http::Parser> parser =
        std::make_unique<http::BasicParser>();
    {
        std::string test_string{"TRACE / HTTP/1.1"};
        auto const result_opt =
            parser->parse_request_line(std::move(test_string));
        ASSERT_TRUE(result_opt.has_value());
        auto const& [request_method, target, version] = result_opt.value();
        EXPECT_EQ(request_method, http::RequestMethod::Trace);
        EXPECT_EQ(target, "/");
        EXPECT_EQ(version, http::ProtocolVersion::OnePointOne);
    }
}

TEST(ParserTest, ParseRequestLineHappyPATCH)
{
    std::unique_ptr<http::Parser> parser =
        std::make_unique<http::BasicParser>();
    {
        std::string test_string{"PATCH / HTTP/1.1"};
        auto const result_opt =
            parser->parse_request_line(std::move(test_string));
        ASSERT_TRUE(result_opt.has_value());
        auto const& [request_method, target, version] = result_opt.value();
        EXPECT_EQ(request_method, http::RequestMethod::Patch);
        EXPECT_EQ(target, "/");
        EXPECT_EQ(version, http::ProtocolVersion::OnePointOne);
    }
}

TEST(ParserTest, ParseRequestLineStrangeCase)
{
    std::unique_ptr<http::Parser> parser =
        std::make_unique<http::BasicParser>();
    {
        std::string test_string{"GeT / HTTP/1.1"};
        auto const result_opt =
            parser->parse_request_line(std::move(test_string));
        ASSERT_TRUE(result_opt.has_value());
        auto const& [request_method, target, version] = result_opt.value();
        EXPECT_EQ(request_method, http::RequestMethod::Get);
        EXPECT_EQ(target, "/");
        EXPECT_EQ(version, http::ProtocolVersion::OnePointOne);
    }
    {
        std::string test_string{"get / HTTP/1.1"};
        auto const result_opt =
            parser->parse_request_line(std::move(test_string));
        ASSERT_TRUE(result_opt.has_value());
        auto const& [request_method, target, version] = result_opt.value();
        EXPECT_EQ(request_method, http::RequestMethod::Get);
        EXPECT_EQ(target, "/");
        EXPECT_EQ(version, http::ProtocolVersion::OnePointOne);
    }
}

TEST(ParserTest, ParseRequestLineIncorrectMethod)
{
    std::unique_ptr<http::Parser> parser =
        std::make_unique<http::BasicParser>();
    {
        std::string test_string{"STRANGE / HTTP/1.1"};
        auto const result_opt =
            parser->parse_request_line(std::move(test_string));
        ASSERT_FALSE(result_opt.has_value());
    }
}

TEST(ParserTest, ParseRequestLineIncorrectVersion)
{
    std::unique_ptr<http::Parser> parser =
        std::make_unique<http::BasicParser>();
    {
        std::string test_string{"GET / HTTP/1.2"};
        auto const result_opt =
            parser->parse_request_line(std::move(test_string));
        ASSERT_FALSE(result_opt.has_value());
    }
}

TEST(ParserTest, ParseRequestLineNoVersion)
{
    std::unique_ptr<http::Parser> parser =
        std::make_unique<http::BasicParser>();
    {
        std::string test_string{"GET /"};
        auto const result_opt =
            parser->parse_request_line(std::move(test_string));
        ASSERT_FALSE(result_opt.has_value());
    }
}

TEST(ParserTest, ParseRequestLineNoMethod)
{
    std::unique_ptr<http::Parser> parser =
        std::make_unique<http::BasicParser>();
    {
        std::string test_string{"/ HTTP/1.1"};
        auto const result_opt =
            parser->parse_request_line(std::move(test_string));
        ASSERT_FALSE(result_opt.has_value());
    }
}

TEST(ParserTest, ParseRequestLineNoTarget)
{
    std::unique_ptr<http::Parser> parser =
        std::make_unique<http::BasicParser>();
    {
        std::string test_string{"GET HTTP/1.1"};
        auto const result_opt =
            parser->parse_request_line(std::move(test_string));
        ASSERT_FALSE(result_opt.has_value());
    }
}

TEST(ParserTest, ParseRequestLineCorrectExtra)
{
    std::unique_ptr<http::Parser> parser =
        std::make_unique<http::BasicParser>();
    {
        std::string test_string{"GET / HTTP/1.1 fjdksa fjfj fdjka ajf"};
        auto const result_opt =
            parser->parse_request_line(std::move(test_string));
        ASSERT_TRUE(result_opt.has_value());
        auto const& [request_method, target, version] = result_opt.value();
        EXPECT_EQ(request_method, http::RequestMethod::Get);
        EXPECT_EQ(target, "/");
        EXPECT_EQ(version, http::ProtocolVersion::OnePointOne);
    }
}
