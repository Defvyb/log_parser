#include "log_controller.h"
#include "thread_pool.h"
#include "utils.hpp"

#include <signal.h>
#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>

inline bool prepareData(std::vector<Log>& logs,
                        std::vector<LogController>& logControllers,
                        std::vector<std::ifstream>& streams,
                        std::optional<ProgramOptions>& options)
{
    for (int i = 0; i < options->filesCount; ++i)
    {
        logControllers.push_back(LogController(logs[i]));

        std::string filename = options->pathToFolder + "file" + std::to_string(i + 1) + ".log";
        streams.emplace_back(filename);
        if (!streams.back().is_open())
        {
            std::cerr << "failed to open file " << filename << std::endl;
            return false;
        }
        std::cout << "will processing " << filename << std::endl;
    }
    return true;
}

std::function<void(int)> shutdownHandler;
void signalHandler(int signal){
    shutdownHandler(signal);
}


int main(int argc, char** argv)
{
    std::atomic<bool> quit = false;
    shutdownHandler = [&quit](int){quit = true;};
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    std::set_terminate([](){ std::cout << "Unhandled exception\n"; std::abort();});

    auto t1 = std::chrono::high_resolution_clock::now();

    std::optional<ProgramOptions> options = getProgramOptions(argc, argv);
    if (!options) return 0;

    std::vector<Log> logs;
    std::vector<LogController> logControllers;
    std::vector<std::ifstream> streams;
    std::vector<bool> results;

    logs.resize(options->filesCount);
    logControllers.reserve(options->filesCount);
    streams.reserve(options->filesCount);
    results.resize(options->filesCount);

    if (!prepareData(logs, logControllers, streams, options)) return -1;

    ThreadPool pool(options->threadsCount);

    for (int i = 0; i < options->filesCount; ++i) //start tasks only when all data prepared, to prevent race condition
    {
        pool.addTask([&logControllers, &streams, &options, i, &results, &quit]() {
            results[i] = logControllers[i].fill(streams[i], *options, &quit);
        });
    }

    while (!pool.isEnded()) std::this_thread::sleep_for(std::chrono::milliseconds(1));

    for (int i = 0; i < options->filesCount; i++)
    {
        if (!results[i])
        {
            std::cerr << "failed to process " << i + 1 << " file" << std::endl;
            return -1;
        }
    }

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
