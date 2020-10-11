#include "gtest/gtest.h"
#include "log_parser/sources/utils.hpp"
#include "log_parser/sources/thread_pool.h"
#include "log_parser/sources/log_controller.h"
#include <limits>
TEST(TEST_PRETTY_DATE_FORMAT_CONVERTER, directCase)
{
    std::array<char, 12> buf;
    ASSERT_STREQ("1970-01-01", getPrettyTime(buf, 0).data());
    ASSERT_STREQ("2020-10-10", getPrettyTime(buf, 1602340036).data());
}

TEST(THREAD_POOL_TEST, directCase)
{
    for(int i=1; i < std::thread::hardware_concurrency() +1; i++)
    {
        ThreadPool pool(i);
        std::atomic<int> var = 0;
        for(int j =0; j<i; j++)
        {
            pool.addTask([&var](){ var++;});
        }

        while(!pool.isEnded()) std::this_thread::sleep_for(std::chrono::milliseconds(1));
        ASSERT_EQ(i, var);
    }
}

TEST(PARSER_TEST, directCase)
{
    std::stringstream ss;

    ss <<  "{\"ts_fact\" :  86401, "
           "\"fact_name\" : \"fact0\", "
           "\"actor_id\": 1, "
           "\"props\": "
           "{ \"prop1\": 1, "
           "\"prop2\": 2, "
           "\"prop3\": 3, "
           "\"prop4\": 4, "
           "\"prop5\": 5, "
           "\"prop6\": 6, "
           "\"prop7\": 7, "
           "\"prop8\": 8, "
           "\"prop9\": 9, "
           "\"prop10\": 10 }}";
    ProgramOptions options;
    options.checkJsonFormat = true;

    Entry entry;
    LogController::fillEntryByLine(ss.str(), entry, options);

    ASSERT_EQ(86400, entry.timestamp);
    ASSERT_STREQ("fact0", entry.factName.c_str());

    int i=1;
    for(auto prop: entry.props)
    {
        ASSERT_EQ(i++, prop);
    }
}

TEST(WRITER_TEST, emptyLogCase)
{
    Log log;

    std::stringstream ss;
    LogController::writeJson(log, ss);

    ASSERT_STREQ("{}", ss.str().c_str());
}

TEST(WRITER_TEST, directCase)
{
    std::stringstream testInputStream;

    testInputStream <<  "{\"ts_fact\" :  86401, "
           "\"fact_name\" : \"fact0\", "
           "\"actor_id\": 1, "
           "\"props\": "
           "{ \"prop1\": 1, "
           "\"prop2\": 2, "
           "\"prop3\": 3, "
           "\"prop4\": 4, "
           "\"prop5\": 5, "
           "\"prop6\": 6, "
           "\"prop7\": 7, "
           "\"prop8\": 8, "
           "\"prop9\": 9, "
           "\"prop10\": 10 }}";

    Log log;
    LogController controller(log);

    ProgramOptions options;
    options.checkJsonFormat = true;

    controller.fill(testInputStream, options);

    std::stringstream testOutputStream;
    LogController::writeJson(log, testOutputStream);

    ASSERT_STREQ("{\n    \"1970-01-02\":"
                 " {\n        \"fact0\": "
                 "[{\n                "
                 "\"props\": [1, 2, 3, 4, 5, 6, 7, 8, 9, 10],\n                "
                 "\"count\": 1\n            }]\n    }\n}", testOutputStream.str().c_str());

}

TEST(WRITER_TEST, propsCountOverfloat)
{
    std::stringstream testInputStream;

    testInputStream <<  "{\"ts_fact\" :  86401, "
           "\"fact_name\" : \"fact0\", "
           "\"actor_id\": 1, "
           "\"props\": "
           "{ \"prop1\": 1, "
           "\"prop2\": 2, "
           "\"prop3\": 3, "
           "\"prop4\": 4, "
           "\"prop5\": 5, "
           "\"prop6\": 6, "
           "\"prop7\": 7, "
           "\"prop8\": 8, "
           "\"prop9\": 9, "
           "\"prop10\": 10 }}";

    Log log;
    Props props = {1,2,3,4,5,6,7,8,9,10};
    log[86400]["fact0"][props] = std::numeric_limits<int>::max();
    LogController controller(log);

    ProgramOptions options;
    options.checkJsonFormat = true;

    controller.fill(testInputStream, options);

    std::stringstream testOutputStream;
    LogController::writeJson(log, testOutputStream);

    ASSERT_STREQ("{\n    \"1970-01-02\":"
                 " {\n        \"fact0\": "
                 "[{\n                "
                 "\"props\": [1, 2, 3, 4, 5, 6, 7, 8, 9, 10],\n                "
                 "\"count\": 2147483647\n            }]\n    }\n}", testOutputStream.str().c_str());

}

TEST(MERGE_TEST, overfloat_count)
{
    std::vector<Log> logs;
    logs.resize(2);

    Props props = {1,2,3,4,5,6,7,8,9,10};
    logs.front()[86400]["fact0"][props] = std::numeric_limits<int>::max();
    logs.back()[86400]["fact0"][props] = 10;

    LogController::mergeLogsIntoFirst(logs);

    ASSERT_EQ(std::numeric_limits<int>::max(), logs.front()[86400]["fact0"][props]);
}

