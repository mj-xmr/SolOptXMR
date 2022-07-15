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
        Assertions::Throw((jsonFile + " failed to parse\n").c_str(), "parseJsonOrThrow");
    }
}

static const rapidjson::Value & GetArrayJson(const EnjoLib::Str & name)
{
    const Str jsonFile = name + ".json";
    rapidjson::Document d;
    {LOGL << "Reading array: " << name << Nl;}
    parseJsonOrThrow(jsonFile, d);
    const rapidjson::Value& array_json = d[name.c_str()];
    //{LOGL << "Reading array: " << name << " succeeded." << Nl;}
    return array_json;
}

static const rapidjson::Value & GetValueJson(const rapidjson::Value & obj, const EnjoLib::Str & identif, const EnjoLib::Str & name)
{
    //{LOGL << "Reading variable: " << name << " of " << identif << Nl;}
    if (not obj.HasMember(name.c_str()))
    {
        const EnjoLib::Str msg = "No member " + name + " in " + identif;
        Assertions::Throw(msg.c_str(), "GetValue");
    }
    return obj[name.c_str()];
}

EnjoLib::Array<BatteryParams> JsonReader::ReadBatteries(bool verbose) const
{
    const CharManipulations cman;
    std::vector<BatteryParams> ret;
    const Str idd = "batteries";
    const rapidjson::Value& arr = GetArrayJson(idd);
    for (auto objIt = arr.Begin(); objIt != arr.End(); ++objIt)
    {
        BatteryParams batObj;
        int count = 1;
        const rapidjson::Value & bat = *objIt;
        ELO

        batObj.DISCHARGE_RATE_C_BY =    GetValueJson(bat, idd, "discharge_rate_c_by").GetInt();
        batObj.MAX_DISCHARGE_AMP =      GetValueJson(bat, idd, "max_discharge_amp").GetDouble();
        batObj.MAX_CAPACITY_AMPH =      GetValueJson(bat, idd, "max_capacity_amph").GetDouble();
        batObj.MIN_LOAD_AMPH =          GetValueJson(bat, idd, "min_load_amph").GetDouble();
        batObj.DISCHARGE_PER_HOUR_PERCENT = GetValueJson(bat, idd, "discharge_per_hour_percent").GetDouble();
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
    const Str idd = "computers";
    const rapidjson::Value& arr = GetArrayJson(idd);
    for (auto compIt = arr.Begin(); compIt != arr.End(); ++compIt)
    {
        Computer compObj;
        int count = 1;
        const rapidjson::Value & comp = *compIt;
        ELO

        compObj.cores =         GetValueJson(comp, idd, "cores").GetInt();
        compObj.wattPerCore =   GetValueJson(comp, idd, "watt_per_core").GetDouble();
        compObj.hashPerCore =   GetValueJson(comp, idd, "hash_per_core").GetDouble();
        /// TODO: hash per core scaling factor
        //compObj.scalingFactor = 0.85;

        compObj.hashPerCore =    GetValueJson(comp, idd, "hash_per_core").GetDouble();
        compObj.wattAsleep =     GetValueJson(comp, idd, "watt_asleep").GetDouble();
        compObj.wattIdle =       GetValueJson(comp, idd, "watt_idle").GetDouble();
        compObj.maxTempCelcius = GetValueJson(comp, idd, "max_temp_celcius").GetDouble();
        compObj.minRunHours =    GetValueJson(comp, idd, "min_run_hours").GetInt();
        compObj.name =           GetValueJson(comp, idd, "name").GetString();
        //LOG <<  << Nl;
        if (comp.HasMember("count"))
        {
            count = GetValueJson(comp, idd, "count").GetInt();
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
    const Str idd = "habits";
    const rapidjson::Value& arr = GetArrayJson(idd);
    for (auto itr = arr.Begin(); itr != arr.End(); ++itr)
    {
        Habit obj;
        int count = 1;
        const rapidjson::Value & habit = *itr;
        ELO

        obj.name = GetValueJson(habit, idd, "name").GetString();
        //{LOGL << obj.name << Nl;}
        obj.watt = GetValueJson(habit, idd, "watt").GetDouble();
        if (habit.HasMember("watt_asleep"))
        {
            obj.watt_asleep = GetValueJson(habit, idd, "watt_asleep").GetDouble();
        }
        if (habit.HasMember("schedule"))
        {
             obj.schedule =         GetValueJson(habit, idd, "schedule").GetString();
             obj.duration_hours =   GetValueJson(habit, idd, "duration_hours").GetDouble();
        }

        //LOG <<  << Nl;
        if (habit.HasMember("count"))
        {
            count = GetValueJson(habit, idd, "count").GetInt();
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
    const Str idd = jsonFile;
    rapidjson::Document d;
    parseJsonOrThrow(jsonFile, d);

    ret.voltage     = GetValueJson(d, idd, "voltage").GetInt();
    ret.generating  = GetValueJson(d, idd, "generate").GetBool();
    ret.buying      = GetValueJson(d, idd, "buy").GetBool();
    ret.selling     = GetValueJson(d, idd, "sell").GetBool();

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
