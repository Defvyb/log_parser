#include <fstream>

int main(int argc, char** argv)//for test use
{

    std::ofstream file("file1.log");

    for (int i = 0; i < 1000000; i++)
    {
        file << "{\"ts_fact\" : " << i
             << ", \"fact_name\" : \"fact" << i / 1000000 << "\""
             << ", \"actor_id\": " << i
             << ", \"props\": { \"prop1\": 1, \"prop2\": 2, \"prop3\": 3, \"prop4\": 4, \"prop5\": 5, \"prop6\": 6, \"prop7\": 7, \"prop8\": 8, \"prop9\": 9, \"prop10\": 10 }}\n";
    }

    return 0;
}
