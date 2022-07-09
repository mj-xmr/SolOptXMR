#include "JsonReader.h"
#include "Computer.h"

#include <Ios/Ifstream.hpp>
#include <Util/CoutBuf.hpp>
#include <Util/FileUtils.hpp>
#include <Util/Tokenizer.hpp>
#include <Util/CharManipulations.hpp>
#include <Statistical/Assertions.hpp>
#include <rapidjson/document.h>

#include <STD/VectorCpp.hpp>

using namespace EnjoLib;

JsonReader::JsonReader(){}
JsonReader::~JsonReader(){}

static void parseJsonOrThrow(const Str & jsonFile, rapidjson::Document & d)
{
    const Str & wholeJson = JsonReader::GetJson(jsonFile);
    if (d.Parse(wholeJson.c_str()).HasParseError())
    {
        Assertions::Throw((jsonFile + " failed to parse\n").c_str(), "GetArray");
    }
}

static const rapidjson::Value & GetArray(const EnjoLib::Str & name)
{
    const Str jsonFile = name + ".json";
    rapidjson::Document d;
    parseJsonOrThrow(jsonFile, d);
    const rapidjson::Value& array_json = d[name.c_str()];
    return array_json;
}

EnjoLib::Array<BatteryParams> JsonReader::ReadBatteries(bool verbose) const
{
    const CharManipulations cman;
    std::vector<BatteryParams> ret;
    const rapidjson::Value& arr = GetArray("batteries");
    for (auto objIt = arr.Begin(); objIt != arr.End(); ++objIt)
    {
        BatteryParams batObj;
        int count = 1;
        const rapidjson::Value & bat = *objIt;
        ELO

        batObj.MAX_DISCHARGE_AMP = bat["max_discharge_amp"].GetDouble();
        batObj.MAX_CAPACITY_AMPH = bat["max_capacity_amph"].GetDouble();
        batObj.MIN_LOAD_AMPH = bat["min_load_amph"].GetDouble();
        batObj.DISCHARGE_PER_HOUR_PERCENT = bat["discharge_per_hour_percent"].GetDouble();
        //batObj.name = bat["name"].GetString();
        //LOG <<  << Nl;
        if (bat.HasMember("count")) /// TODO: Repeated pattern
        {
            count = bat["count"].GetInt();
        }
        if (count == 0)
        {
            continue; // Disabled, yet still registered.
        }
        if (count > 1)
        {
            batObj.MAX_CAPACITY_AMPH *= count;
            batObj.MIN_LOAD_AMPH *= count;
        }
        ret.push_back(batObj);
    }
    if (verbose)
    {
        ELO
        LOG << "Available batteries:\n";
        for (const BatteryParams & bat : ret)
        {
            LOG << bat.Print() << Nl;
        }
    }
    return ret;
}

EnjoLib::Array<Computer> JsonReader::ReadComputers(bool verbose) const
{
    const CharManipulations cman;
    std::vector<Computer> ret;
    const rapidjson::Value& arr = GetArray("computers");
    for (auto compIt = arr.Begin(); compIt != arr.End(); ++compIt)
    {
        Computer compObj;
        int count = 1;
        const rapidjson::Value & comp = *compIt;
        ELO

        compObj.cores = comp["cores"].GetInt();
        compObj.wattPerCore = comp["watt_per_core"].GetDouble();
        compObj.hashPerCore = comp["hash_per_core"].GetDouble();
        /// TODO: hash per core scaling factor
        //compObj.scalingFactor = 0.85;

        compObj.hashPerCore = comp["hash_per_core"].GetDouble();
        compObj.wattAsleep = comp["watt_asleep"].GetDouble();
        compObj.wattIdle = comp["watt_idle"].GetDouble();
        compObj.maxTempCelcius = comp["max_temp_celcius"].GetDouble();
        compObj.minRunHours = comp["min_run_hours"].GetInt();
        compObj.name = comp["name"].GetString();
        //LOG <<  << Nl;
        if (comp.HasMember("count"))
        {
            count = comp["count"].GetInt();
        }
        if (count == 0)
        {
            continue; // Disabled, yet still registered.
        }
        ret.push_back(compObj);
        if (count > 1)
        {
            const Str origName = compObj.name;
            // Multiple instances of the same type.
            for (int i = 1; i < count; ++i)
            {
                const Str suffix = "_" + cman.ToStr(i + 1);
                compObj.name = origName + suffix;
                ret.push_back(compObj);
            }
        }

    }
    if (verbose)
    {
        ELO
        LOG << "Available computers:\n";
        for (const Computer & comp : ret)
        {
            LOG << comp.Print() << Nl;
        }
    }
    return ret;
}

EnjoLib::Array<Habit> JsonReader::ReadHabits(bool verbose) const
{
    const CharManipulations cman;
    std::vector<Habit> ret;
    const rapidjson::Value& arr = GetArray("habits");
    for (auto itr = arr.Begin(); itr != arr.End(); ++itr)
    {
        Habit obj;
        int count = 1;
        const rapidjson::Value & habit = *itr;
        ELO
        
        obj.name = habit["name"].GetString();
        {LOGL << obj.name << Nl;}
        obj.watt = habit["watt"].GetDouble();
        if (habit.HasMember("watt_asleep"))
        {
            obj.watt_asleep = habit["watt_asleep"].GetDouble();
        }
        if (habit.HasMember("schedule"))
        {
             obj.schedule = habit["schedule"].GetString();
             obj.duration_hours = habit["duration_hours"].GetDouble();
        }
        
        //LOG <<  << Nl;
        if (habit.HasMember("count"))
        {
            count = habit["count"].GetInt();
        }
        if (count == 0)
        {
            continue; // Disabled, yet still registered.
        }
        if (count > 1)
        {
            obj.watt *= count;
            obj.watt_asleep *= count;
        }
        ret.push_back(obj);
    }
    if (verbose)
    {
        ELO
        LOG << "Available habits:\n";
        for (const auto & obj : ret)
        {
            LOG << obj.Print() << Nl;
        }
    }
    for (auto & obj : ret)
    {
        obj.ParseSchedule();
    }
    return ret;
}

System JsonReader::ReadSystem(bool verbose) const
{
    System ret;
    const Str jsonFile = "system.json";
    rapidjson::Document d;
    parseJsonOrThrow(jsonFile, d);
    
    ret.voltage     = d["voltage"].GetInt();
    ret.generating  = d["generate"].GetBool();
    ret.buying      = d["buy"].GetBool();
    ret.selling     = d["sell"].GetBool();
    
    return ret;
}

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

EnjoLib::Str JsonReader::GetJson(const EnjoLib::Str & fileName)
{
    const char *homedir = nullptr;

    if ((homedir = getenv("HOME")) == NULL) {
        homedir = getpwuid(getuid())->pw_dir;
    }
    if (homedir == nullptr)
    {
        Assertions::Throw("Not found home dir", "JsonReader::GetJson");
    }
    Ifstream fcomps(Str(homedir) + "/.config/solar/" + fileName);
    const Tokenizer tok;
    const VecStr & lines = tok.GetLines(fcomps);
    Str wholeJson;
    for (const Str & line : lines ) wholeJson += line;

    return wholeJson;
}

/*
    {
    // 1. Parse a JSON string into DOM.
    const char* json = "{\"project\":\"rapidjson\",\"stars\":10}";
    rapidjson::Document d;
    d.Parse(json);

    // 2. Modify it by DOM.
    const rapidjson::Value& s = d["stars"];
    LOGL << s.GetInt() << Nl;

    }
    */
