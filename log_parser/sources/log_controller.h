#pragma once
#include "types.hpp"
#include <istream>
#include <ostream>
#include <vector>

class LogController final
{
public:
    explicit LogController(Log& log);
    ~LogController() noexcept;
    LogController(LogController&&) = default;

    LogController(LogController&) = delete;
    LogController& operator=(LogController&) = delete;
    LogController& operator=(LogController&&) = delete;

    bool fill(std::istream& stream, const ProgramOptions& options);

    static void mergeLogsIntoFirst(std::vector<Log>& logs);
    static void writeJson(const Log& log, std::ostream& ostream);

    static bool fillEntryByLine(std::string_view line, Entry& entry, const ProgramOptions& options);

private:

    Log& m_log;
    static std::array<std::string, std::tuple_size<Props>::value> propsNames;
};
