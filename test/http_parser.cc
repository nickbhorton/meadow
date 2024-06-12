#include <memory>

#include <gtest/gtest.h>

#include "basic_http_parser.h"

TEST(HTTPParserTest, SplitHappy)
{
    std::unique_ptr<HTTPParser> parser = std::make_unique<BasicHTTPParser>();
    {
        std::string test_string{
            "This is a happy case test.\nThere will be no glaring "
            "issues with the string.\nThere will not be any empty "
            "lines\nand it will not end with a newline"
        };
        std::vector<std::string> parsed_strings =
            parser->split(std::move(test_string));

        ASSERT_EQ(parsed_strings.size(), 4)
            << "Did not parser the correct number of string";

        EXPECT_STREQ("This is a happy case test.", parsed_strings[0].c_str());
        EXPECT_STREQ(
            "There will be no glaring issues with the string.",
            parsed_strings[1].c_str()
        );
        EXPECT_STREQ(
            "There will not be any empty lines",
            parsed_strings[2].c_str()
        );
        EXPECT_STREQ(
            "and it will not end with a newline",
            parsed_strings[3].c_str()
        );
    }

    {
        // with \r\n
        std::string test_string{
            "This is a happy case test.\r\nThere will be no glaring "
            "issues with the string.\r\nThere will not be any empty "
            "lines\r\nand it will not end with a newline"
        };
        std::vector<std::string> parsed_strings =
            parser->split(std::move(test_string));

        ASSERT_EQ(parsed_strings.size(), 4)
            << "Did not parser the correct number of string";

        EXPECT_STREQ("This is a happy case test.", parsed_strings[0].c_str());
        EXPECT_STREQ(
            "There will be no glaring issues with the string.",
            parsed_strings[1].c_str()
        );
        EXPECT_STREQ(
            "There will not be any empty lines",
            parsed_strings[2].c_str()
        );
        EXPECT_STREQ(
            "and it will not end with a newline",
            parsed_strings[3].c_str()
        );
    }
}

TEST(HTTPParserTest, SplitEndNewline)
{
    std::unique_ptr<HTTPParser> parser = std::make_unique<BasicHTTPParser>();
    {
        std::string test_string{
            "This is a test.\nThere will be a glaring "
            "issues with the string.\nIf the request ends with a "
            "newline there will not be an empty string at the end\n"
        };
        std::vector<std::string> parsed_strings =
            parser->split(std::move(test_string));

        ASSERT_EQ(parsed_strings.size(), 3)
            << "Did not parser the correct number of string";

        EXPECT_STREQ("This is a test.", parsed_strings[0].c_str());
        EXPECT_STREQ(
            "There will be a glaring issues with the string.",
            parsed_strings[1].c_str()
        );
        EXPECT_STREQ(
            "If the request ends with a newline there will not be an "
            "empty string at the end",
            parsed_strings[2].c_str()
        );
    }

    {
        // with \r\n
        std::string test_string{
            "This is a test.\r\nThere will be a glaring "
            "issues with the string.\r\nIf the request ends with a "
            "newline there will not be an empty string at the end\r\n"
        };
        std::vector<std::string> parsed_strings =
            parser->split(std::move(test_string));

        ASSERT_EQ(parsed_strings.size(), 3)
            << "Did not parser the correct number of string";

        EXPECT_STREQ("This is a test.", parsed_strings[0].c_str());
        EXPECT_STREQ(
            "There will be a glaring issues with the string.",
            parsed_strings[1].c_str()
        );
        EXPECT_STREQ(
            "If the request ends with a newline there will not be an "
            "empty string at the end",
            parsed_strings[2].c_str()
        );
    }
}

TEST(HTTPParserTest, SplitDoubleNewline)
{
    std::unique_ptr<HTTPParser> parser = std::make_unique<BasicHTTPParser>();
    {
        std::string test_string{
            "This is the head\r\n\r\nThis is the payload\r\nthis will be in "
            "the same string as the payload\r\n"
        };
        std::vector<std::string> parsed_strings =
            parser->split(std::move(test_string));
        ASSERT_EQ(parsed_strings.size(), 2)
            << "Did not parser the correct number of string";

        EXPECT_STREQ("This is the head", parsed_strings[0].c_str());
        EXPECT_STREQ(
            "This is the payload\r\nthis will be in "
            "the same string as the payload\r\n",
            parsed_strings[1].c_str()
        );
    }

    {
        // with \r\n
        std::string test_string{
            "This is the head\n\nThis is the payload\nthis will be in "
            "the same string as the payload\n"
        };
        std::vector<std::string> parsed_strings =
            parser->split(std::move(test_string));
        ASSERT_EQ(parsed_strings.size(), 2)
            << "Did not parser the correct number of string";

        EXPECT_STREQ("This is the head", parsed_strings[0].c_str());
        EXPECT_STREQ(
            "This is the payload\nthis will be in "
            "the same string as the payload\n",
            parsed_strings[1].c_str()
        );
    }
}

TEST(HTTPParserTest, SplitEmptyString)
{
    std::unique_ptr<HTTPParser> parser = std::make_unique<BasicHTTPParser>();
    std::string test_string{};
    std::vector<std::string> parsed_strings =
        parser->split(std::move(test_string));
    ASSERT_EQ(parsed_strings.size(), 0)
        << "Did not parser the correct number of string";
}

TEST(HTTPParserTest, SplitNoNewLine)
{
    std::unique_ptr<HTTPParser> parser = std::make_unique<BasicHTTPParser>();
    std::string test_string{"this is a single line."};
    std::vector<std::string> parsed_strings =
        parser->split(std::move(test_string));
    ASSERT_EQ(parsed_strings.size(), 1)
        << "Did not parser the correct number of string";
    EXPECT_STREQ("this is a single line.", parsed_strings[0].c_str());
}

TEST(HTTPParserTest, SplitNoHeader)
{
    std::unique_ptr<HTTPParser> parser = std::make_unique<BasicHTTPParser>();
    {
        std::string test_string{"\n\nNo header\nmultiple lines."};
        std::vector<std::string> parsed_strings =
            parser->split(std::move(test_string));
        ASSERT_EQ(parsed_strings.size(), 1)
            << "Did not parser the correct number of string";
        EXPECT_STREQ("No header\nmultiple lines.", parsed_strings[0].c_str());
    }

    {
        // with \r\n
        std::string test_string{"\r\n\r\nNo header\r\nmultiple lines."};
        std::vector<std::string> parsed_strings =
            parser->split(std::move(test_string));
        ASSERT_EQ(parsed_strings.size(), 1)
            << "Did not parser the correct number of string";
        EXPECT_STREQ("No header\r\nmultiple lines.", parsed_strings[0].c_str());
    }
}

TEST(HTTPParserTest, SplitHTTPExample)
{
    std::unique_ptr<HTTPParser> parser = std::make_unique<BasicHTTPParser>();
    {
        std::string test_string{
            "POST /cgi-bin/process.cgi HTTP/1.1\n"
            "User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\n"
            "Host: www.tutorialspoint.com\n"
            "Content-Type: text/xml; charset=utf-8\n"
            "Content-Length: length\n"
            "Accept-Language: en-us\n"
            "Accept-Encoding: gzip, deflate\n"
            "Connection: Keep-Alive\n"
            "\n"
            "<?xml version=\" 1.0 \" encoding=\" utf - 8 \"?>\n"
            "<string xmlns=\" http : // clearforest.com/\">string</string>\n"
        };
        std::vector<std::string> parsed_strings =
            parser->split(std::move(test_string));
        ASSERT_EQ(parsed_strings.size(), 9)
            << "Did not parser the correct number of string";
        EXPECT_STREQ(
            "POST /cgi-bin/process.cgi HTTP/1.1",
            parsed_strings[0].c_str()
        );
        EXPECT_STREQ(
            "User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)",
            parsed_strings[1].c_str()
        );
        EXPECT_STREQ("Host: www.tutorialspoint.com", parsed_strings[2].c_str());
        EXPECT_STREQ(
            "Content-Type: text/xml; charset=utf-8",
            parsed_strings[3].c_str()
        );
        EXPECT_STREQ("Content-Length: length", parsed_strings[4].c_str());
        EXPECT_STREQ("Accept-Language: en-us", parsed_strings[5].c_str());
        EXPECT_STREQ(
            "Accept-Encoding: gzip, deflate",
            parsed_strings[6].c_str()
        );
        EXPECT_STREQ("Connection: Keep-Alive", parsed_strings[7].c_str());
        EXPECT_STREQ(
            "<?xml version=\" 1.0 \" encoding=\" utf - 8 \"?>\n"
            "<string xmlns=\" http : // clearforest.com/\">string</string>\n",
            parsed_strings[8].c_str()
        );
    }
    {
        // with \r\n
        std::string test_string{
            "POST /cgi-bin/process.cgi HTTP/1.1\r\n"
            "User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\n"
            "Host: www.tutorialspoint.com\r\n"
            "Content-Type: text/xml; charset=utf-8\r\n"
            "Content-Length: length\r\n"
            "Accept-Language: en-us\r\n"
            "Accept-Encoding: gzip, deflate\r\n"
            "Connection: Keep-Alive\r\n"
            "\r\n"
            "<?xml version=\" 1.0 \" encoding=\" utf - 8 \"?>\n"
            "<string xmlns=\" http : // clearforest.com/\">string</string>\n"
        };
        std::vector<std::string> parsed_strings =
            parser->split(std::move(test_string));
        ASSERT_EQ(parsed_strings.size(), 9)
            << "Did not parser the correct number of string";
        EXPECT_STREQ(
            "POST /cgi-bin/process.cgi HTTP/1.1",
            parsed_strings[0].c_str()
        );
        EXPECT_STREQ(
            "User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)",
            parsed_strings[1].c_str()
        );
        EXPECT_STREQ("Host: www.tutorialspoint.com", parsed_strings[2].c_str());
        EXPECT_STREQ(
            "Content-Type: text/xml; charset=utf-8",
            parsed_strings[3].c_str()
        );
        EXPECT_STREQ("Content-Length: length", parsed_strings[4].c_str());
        EXPECT_STREQ("Accept-Language: en-us", parsed_strings[5].c_str());
        EXPECT_STREQ(
            "Accept-Encoding: gzip, deflate",
            parsed_strings[6].c_str()
        );
        EXPECT_STREQ("Connection: Keep-Alive", parsed_strings[7].c_str());
        EXPECT_STREQ(
            "<?xml version=\" 1.0 \" encoding=\" utf - 8 \"?>\n"
            "<string xmlns=\" http : // clearforest.com/\">string</string>\n",
            parsed_strings[8].c_str()
        );
    }
}

TEST(HTTPParserTest, SplitJustStartLine)
{
    std::unique_ptr<HTTPParser> parser = std::make_unique<BasicHTTPParser>();
    {
        std::string test_string{"POST /cgi-bin/process.cgi HTTP/1.1\n"};
        std::vector<std::string> parsed_strings =
            parser->split(std::move(test_string));
        ASSERT_EQ(parsed_strings.size(), 1)
            << "Did not parser the correct number of string";
        EXPECT_STREQ(
            "POST /cgi-bin/process.cgi HTTP/1.1",
            parsed_strings[0].c_str()
        );
    }
    {
        std::string test_string{"POST /cgi-bin/process.cgi HTTP/1.1\r\n"};
        std::vector<std::string> parsed_strings =
            parser->split(std::move(test_string));
        ASSERT_EQ(parsed_strings.size(), 1)
            << "Did not parser the correct number of string";
        EXPECT_STREQ(
            "POST /cgi-bin/process.cgi HTTP/1.1",
            parsed_strings[0].c_str()
        );
    }
    {
        std::string test_string{"POST /cgi-bin/process.cgi HTTP/1.1"};
        std::vector<std::string> parsed_strings =
            parser->split(std::move(test_string));
        ASSERT_EQ(parsed_strings.size(), 1)
            << "Did not parser the correct number of string";
        EXPECT_STREQ(
            "POST /cgi-bin/process.cgi HTTP/1.1",
            parsed_strings[0].c_str()
        );
    }
}
