#include <gtest/gtest.h>
#include <memory>

#include "basic_http_parser.h"

TEST(HTTPParserTest, SplitHappy)
{
    std::unique_ptr<HTTPParser> parser = std::make_unique<BasicHTTPParser>();
    std::string s1{"This is a happy case test.\nThere will be no glaring "
                   "issues with the string.\nThere will not be any empty "
                   "lines\nand it will not end with a newline"};
    std::vector<std::string> parsed_strings = parser->split(std::move(s1));
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
