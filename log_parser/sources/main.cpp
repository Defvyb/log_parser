#include "log_controller.h"
#include <fstream>
#include <iostream>
#include <chrono>

int main(int argc, char ** argv)
{
    Log log;

    std::ifstream file("file.txt");
    if(!file.is_open())
    {
        std::cerr << "failed to open file.txt" << std::endl;
        return -1;
    }

    auto t1 = std::chrono::high_resolution_clock::now();

    ProgramOptions options;
    options.checkJsonFormat = false;

    LogController logController(log);
    if(!logController.fill(file, options))
    {
        return -1;
    }
    logController.writeJson(std::ofstream("agr.txt"));

    std::cout << "parse done" << std::endl;
    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout << log.size() <<" " <<std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count() << " ms" << std::endl;

    return 0;
}




