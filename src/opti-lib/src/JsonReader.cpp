#include "JsonReader.h"
#include "Computer.h"

#include <Ios/Ifstream.hpp>
#include <Util/CoutBuf.hpp>
#include <Util/FileUtils.hpp>
#include <Util/Tokenizer.hpp>
#include <Util/CharManipulations.hpp>
#include <rapidjson/document.h>

#include <STD/VectorCpp.hpp>

using namespace EnjoLib;

JsonReader::JsonReader(){}
JsonReader::~JsonReader(){}

EnjoLib::Array<BatteryParams> JsonReader::ReadBatteries(bool verbose) const
{
    std::vector<BatteryParams> ret;
    const Str & wholeJson = GetJson("batteries.json");

    const CharManipulations cman;
    rapidjson::Document d;
    d.Parse(wholeJson.c_str());

    const rapidjson::Value& batteriesVal = d["batteries"];
    for (auto objIt = batteriesVal.Begin(); objIt != batteriesVal.End(); ++objIt)
    {
        /// TODO: Until here this could be abstracted

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
    std::vector<Computer> ret;
    const Str & wholeJson = GetJson("computers.json");
    const CharManipulations cman;
    rapidjson::Document d;
    d.Parse(wholeJson.c_str());

    const rapidjson::Value& computersVal = d["computers"];
    for (auto compIt = computersVal.Begin(); compIt != computersVal.End(); ++compIt)
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
            // Multiple instances of the same type.
            for (int i = 1; i < count; ++i)
            {
                Computer compObjNext = compObj;
                const Str suffix = "_" + cman.ToStr(i + 1);
                compObjNext.name += suffix;
                ret.push_back(compObjNext);
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

System JsonReader::ReadSystem(bool verbose) const
{
    System ret;
    const Str & wholeJson = GetJson("system.json");
    const CharManipulations cman;
    rapidjson::Document d;
    d.Parse(wholeJson.c_str());
    
    ret.voltage = d["voltage"].GetInt();
    ret.type    = d["type"].GetString();
    
    return ret;
}

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

EnjoLib::Str JsonReader::GetJson(const EnjoLib::Str & fileName) const
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
