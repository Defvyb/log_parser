#include "gtest/gtest.h"
#include "log_parser/sources/utils.hpp"

TEST(TEST_PRETTY_DATE_FORMAT_CONVERTER, directCase)
{
    std::array<char, 12> buf;
    ASSERT_STREQ("1970-01-01", getPrettyTime(buf, 0).data());
    ASSERT_STREQ("2020-10-10", getPrettyTime(buf, 1602340036).data());
}
