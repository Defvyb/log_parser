#pragma once
#include <cstdint>
#include <time.h>
#include <array>
#include <cassert>

template<typename T>
const T & getPrettyTime(T & buf, uint32_t timeStamp)
{
    assert(buf.size() > 11);
    time_t rawtime = timeStamp;
    struct tm * timeinfo;
    timeinfo = localtime (&rawtime);

    strftime(buf.data(), buf.size(), "%Y-%m-%d", timeinfo);
    return buf;
}




