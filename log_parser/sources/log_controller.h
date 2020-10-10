#pragma once
#include "types.hpp"
#include <istream>
#include <ostream>

class LogController final
{
public:
    LogController(Log & log);
    ~LogController();

    LogController(LogController &) = delete;
    LogController(LogController &&) = delete;
    LogController& operator=(LogController&) = delete;
    LogController& operator=(LogController&&) = delete;

    bool fill(std::istream & stream, const ProgramOptions & options);
    void writeJson(std::ostream && ostream);

private:
    template <typename T>
    bool fillEntryByLine(T & line, Entry & entry, const ProgramOptions & options);

    Log & m_log;
    static std::array<std::string, std::tuple_size<Props>::value> propsNames;
};


