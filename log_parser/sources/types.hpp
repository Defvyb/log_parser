#pragma once

#include <array>
#include <map>
#include <string>
#include <unordered_map>

using DaysSinceEpoch = int;
using FaktName       = std::string;
using Props          = std::array<int, 10>; //don't forget to edit Log_controller::propsNames
using PropsCount     = int;

template<typename Container>
struct ContainerHash {
    std::size_t operator()(Container const& c) const
    {
        std::size_t resultXor = 0;
        for (auto val : c)
        {
            resultXor ^= val;
        }
        return resultXor;
    }
};

using Log = std::map<DaysSinceEpoch, std::unordered_map<FaktName, std::unordered_map<Props, int, ContainerHash<Props> > > >;

struct Entry {
    int timestamp;
    std::string factName;
    Props props;
};

struct ProgramOptions {
    bool checkJsonFormat = false;
    int numFiles         = 1;
    int threadsCount     = 1;
    std::string pathToFolder;
};
