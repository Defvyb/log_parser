#include "log_controller.h"

#include <rapidjson/include/rapidjson/document.h>

#include <iostream>

#include "rapidjson/include/rapidjson/ostreamwrapper.h"
#include "rapidjson/include/rapidjson/prettywriter.h"
#include "utils.hpp"

std::array<std::string, std::tuple_size<Props>::value>
        LogController::propsNames = {"prop1", "prop2", "prop3", "prop4", "prop5",
                                     "prop6", "prop7", "prop8", "prop9", "prop10"};

LogController::LogController(Log& log) :
        m_log(log) {}
LogController::~LogController() {}

bool LogController::fill(std::istream& stream, const ProgramOptions& options, const std::atomic<bool>* quit)
{
    std::array<char, 1024> line;
    while (stream.getline(line.data(), line.size()))
    {
        if (quit && *quit)
        {
            return false;
        }

        Entry entry;
        if (!fillEntryByLine(line.data(), entry, options))
        {
            return false;
        }
        auto& propsCount = m_log[entry.timestamp][entry.factName][entry.props];
        if (propsCount != INT_MAX)
        {
            propsCount += 1;
        }
    }
    return true;
}

bool LogController::fillEntryByLine(std::string_view line,
                                    Entry& entry,
                                    const ProgramOptions& options)
{
    rapidjson::Document d;
    d.Parse(line.data());

    if (options.checkJsonFormat)
    {
        if (d.HasParseError())
        {
            return false;
        }

        if (!d.HasMember("ts_fact") || !d["ts_fact"].IsInt() ||
            !d.HasMember("fact_name") || !d["fact_name"].IsString() ||
            !d.HasMember("props") || !d["props"].IsObject())
        {
            return false;
        }
    }

    entry.timestamp = d["ts_fact"].GetInt();
    constexpr int SECONDS_IN_DAY = 60 * 60 * 24;
    entry.timestamp /= SECONDS_IN_DAY;
    entry.timestamp *= SECONDS_IN_DAY;
    entry.factName = d["fact_name"].GetString();

    for (int i = 0; i < std::tuple_size<Props>::value; ++i)
    {
        if (options.checkJsonFormat &&
            (!d["props"].HasMember(propsNames[i].c_str()) ||
             !d["props"][propsNames[i].c_str()].IsInt()))
        {
            return false;
        }
        entry.props[i] = d["props"][propsNames[i].c_str()].GetInt();
    }

    return true;
}

void LogController::writeJson(const Log& log, std::ostream& ostream)
{
    rapidjson::OStreamWrapper streamWrapper(ostream);
    rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(streamWrapper);
    writer.SetFormatOptions(rapidjson::kFormatSingleLineArray);
    writer.StartObject();

    std::array<char, 12> date;

    for (auto timestamp : log) {
        writer.Key(getPrettyTime(date, timestamp.first).data());
        writer.StartObject();

        for (auto fakt : timestamp.second)
        {
            writer.Key(fakt.first.c_str());
            writer.StartArray();

            for (auto props : fakt.second)
            {
                writer.StartObject();
                writer.Key("props");
                writer.StartArray();

                for (int i = 0; i < std::tuple_size<Props>::value; i++)
                {
                    writer.Int(props.first[i]);
                }

                writer.EndArray();
                writer.Key("count");
                writer.Int(props.second);
                writer.EndObject();
            }
            writer.EndArray();
        }
        writer.EndObject();
    }
    writer.EndObject();
}

void LogController::mergeLogsIntoFirst(std::vector<Log>& logs)
{
    if (logs.size() < 2)
        return;

    auto& firstLog = logs.front();
    for (auto it = std::next(logs.begin(), 1); it != logs.end(); ++it)
    {
        for (auto timestamp : *it) {
            for (auto fakt : timestamp.second)
            {
                for (auto props : fakt.second)
                {
                    auto& propsCount     = firstLog[timestamp.first][fakt.first][props.first];
                    int propsCountBefore = propsCount;
                    propsCount += props.second;
                    if ((propsCount - props.second) != propsCountBefore)
                    {
                        propsCount = INT_MAX;
                    }
                }
            }
        }
    }
    logs.erase(std::next(logs.cbegin(), 1), logs.cend());
}
