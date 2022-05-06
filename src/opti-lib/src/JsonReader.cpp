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

EnjoLib::Array<Computer> JsonReader::ReadComputers(bool verbose) const
{
    std::vector<Computer> ret;
    const Str & fileJson = "computers.json";
    Str path = fileJson;
    if (not FileUtils().FileExists(path))
    {
        path = "../../../" + fileJson;
    }
    if (not FileUtils().FileExists(path))
    {
        path = "../" + fileJson;
    }
    Ifstream fcomps(path);
    const Tokenizer tok;
    const VecStr & lines = tok.GetLines(fcomps);
    Str wholeJson;
    for (const Str & line : lines ) wholeJson += line;

    const CharManipulations cman;
    rapidjson::Document d;
    d.Parse(wholeJson.c_str());

    const rapidjson::Value& computersVal = d["computers"];
    //const auto & computers = computersVal.
    for (auto compIt = computersVal.Begin(); compIt != computersVal.End(); ++compIt)
    {
        Computer compObj;
        int count = 1;
        const rapidjson::Value & comp = *compIt;
        ELO

        compObj.cores = comp["cores"].GetInt();
        compObj.wattPerCore = comp["watt_per_core"].GetDouble();
        compObj.hashPerCore = comp["hash_per_core"].GetDouble();
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

        ret.push_back(compObj);
        if (count > 1)
        {
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
