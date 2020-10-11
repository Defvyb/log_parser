#include "log_controller.h"
#include "thread_pool.h"
#include "utils.hpp"

#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>

int main(int argc, char** argv)
{
    auto t1 = std::chrono::high_resolution_clock::now();

    std::optional<ProgramOptions> options = getProgramOptions(argc,argv);
    if(!options) return 0;

    std::vector<Log> logs;
    logs.resize(options->numFiles);

    std::vector<LogController> logControllers;
    logControllers.reserve(options->numFiles);

    std::vector<std::ifstream> streams;
    streams.reserve(options->numFiles);

    ThreadPool pool(options->threadsCount);

    for (int i = 0; i < options->numFiles; ++i)
    {
        logControllers.push_back(LogController(logs[i]));

        std::string filename = options->pathToFolder + "file" + std::to_string(i + 1) + ".log";
        streams.emplace_back(filename);
        if (!streams.back().is_open())
        {
            std::cerr << "failed to open file" << filename;
            return -1;
        }
        std::cout << "adding " << filename << std::endl;
    }

    for (int i = 0; i < options->numFiles; ++i) //start tasks only when all data prepared, to prevent race condition
    {
        pool.addTask([&logControllers, &streams, &options, i]() {
            logControllers[i].fill(streams[i], *options);
        });
    }

    while (!pool.isEnded()) std::this_thread::sleep_for(std::chrono::milliseconds(1));

    LogController::mergeLogsIntoFirst(logs);

    const char OUT_FILE_NAME[] = "agr.txt";
    std::ofstream jsonToWrite(OUT_FILE_NAME);
    if (!jsonToWrite.is_open())
    {
        std::cerr << "failed to open " << OUT_FILE_NAME << " to write" << std::endl;
    }

    LogController::writeJson(logs.front(), jsonToWrite);

    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout << "time: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms" << std::endl;

    return 0;
}
