#pragma once
#include "types.hpp"

#include <time.h>

#include <cassert>
#include <cstdint>
#include <optional>
#include <iostream>
#include <unordered_map>


template<typename T>
const T& getPrettyTime(T& buf, uint32_t timeStamp)
{
    assert(buf.size() > 11);
    time_t rawtime = timeStamp;
    struct tm timeinfo;
    localtime_r(&rawtime, &timeinfo);

    strftime(buf.data(), buf.size(), "%Y-%m-%d", &timeinfo);
    return buf;
}

//т.к. boost использовать нельзя, напишем не самый изящный коленочный парсинг параметров

inline void printHelp()
{
    std::cout << "Usage:  -p=/path/to/logs path to logs, default is current dir \n";
    std::cout << "        -n=1 files count, default is 1 \n";
    std::cout << "        -t=1 threads count, default is 1 \n";
    std::cout << "        -c json checking enabled(it can extremely slow entire process) \n";
    std::cout << "        -h this help  \n";
}

inline std::optional<ProgramOptions> getProgramOptions(int argc, char* argv[]) noexcept
{
    ProgramOptions options;
    for (int i = 1; i < argc; i++) {
        std::string arg(argv[i]);
        auto findResult = arg.find("-h");
        if (findResult != std::string::npos)
        {
            printHelp();
            return std::nullopt;
        }

        options.checkJsonFormat = (arg.find("-c") != std::string::npos);

        findResult = arg.find("-n");
        if (findResult != std::string::npos)
        {
            options.filesCount = std::atoi(arg.substr(arg.find_first_of("=") + 1).c_str());
        }

        findResult = arg.find("-t");
        if (findResult != std::string::npos)
        {
            options.threadsCount = std::atoi(arg.substr(arg.find_first_of("=") + 1).c_str());
        }

        findResult = arg.find("-p");
        if (findResult != std::string::npos)
        {
            options.pathToFolder = arg.substr(arg.find_first_of("=") + 1);
        }
    }

    if(options.filesCount <= 0)
    {
        std::cerr << "files count must be greater than 0" << std::endl;
        return std::nullopt;
    }

    if(options.threadsCount <= 0)
    {
        std::cerr << "threads count must be greater than 0" << std::endl;
        return std::nullopt;
    }

    if (options.threadsCount > options.filesCount)
    {
        options.threadsCount = options.filesCount;
        std::cout << "setting threads count equal to files count: " << options.threadsCount << std::endl;
    }

    return options;
}
