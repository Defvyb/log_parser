#include "log_controller.h"
#include "utils.hpp"

#include <iostream>
#include <rapidjson/include/rapidjson/document.h>
#include "rapidjson/include/rapidjson/prettywriter.h"
#include "rapidjson/include/rapidjson/ostreamwrapper.h"


std::array<std::string, std::tuple_size<Props>::value> LogController::propsNames = {"prop1","prop2","prop3","prop4","prop5","prop6","prop7","prop8","prop9","prop10"};

LogController::LogController(Log & log):m_log(log){}
LogController::~LogController(){}

bool LogController::fill(std::istream & stream, const ProgramOptions & options)
{
    std::array<char, 1024> line;
    while (stream.getline(line.data(), line.size()))
    {
        Entry entry;
        if(!fillEntryByLine(line, entry, options))
        {
            std::cerr << "bad line: " << line.data()<< std::endl;
            return -1;
        }

        m_log[entry.timestamp][entry.factName][entry.props] += 1;
    }
}

template <typename T>
bool LogController::fillEntryByLine(T & line, Entry & entry, const ProgramOptions & options)
{
    rapidjson::Document d;
    d.Parse(line.data());

    if(options.checkJsonFormat)
    {
        if(d.HasParseError())
        {
            return false;
        }

        if(!d.HasMember("ts_fact") ||
           !d["ts_fact"].IsInt() ||
           !d.HasMember("fact_name") ||
           !d["fact_name"].IsString() ||
           !d.HasMember("props") ||
           !d["props"].IsObject())
        {
            return false;
        }
    }

    entry.timestamp = d["ts_fact"].GetInt();
    constexpr int SECONDS_IN_DAY = 60*60*24;
    entry.timestamp /= SECONDS_IN_DAY;
    entry.timestamp *= SECONDS_IN_DAY;
    entry.factName = d["fact_name"].GetString();

    for(int i=0; i < entry.props.size(); ++i)
    {
        if(options.checkJsonFormat)
        {
            if(!d["props"].HasMember(propsNames[i].c_str()) || !d["props"][propsNames[i].c_str()].IsInt())
            {
                return false;
            }
        }
        entry.props[i] = d["props"][propsNames[i].c_str()].GetInt();
    }

    return true;
}

void LogController::writeJson(std::ostream && ostream)
{
    rapidjson::OStreamWrapper streamWrapper(ostream);
    rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(streamWrapper);
    writer.SetFormatOptions(rapidjson:: kFormatSingleLineArray);
    writer.StartObject();

    std::array<char, 12> date;

    for(auto timestamp: m_log)
    {
        writer.Key(getPrettyTime(date, timestamp.first).data());
        writer.StartObject();
        for(auto fakt: timestamp.second)
        {
            writer.Key(fakt.first.c_str());
            writer.StartArray();
            for(auto props: fakt.second)
            {
                writer.StartObject();
                writer.Key("props");
                writer.StartArray();
                for(int i=0; i < std::tuple_size<Props>::value; i++)
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

