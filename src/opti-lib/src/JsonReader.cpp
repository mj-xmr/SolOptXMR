#include "JsonReader.h"
#include "Computer.h"
#include "ConfigSol.h"

#include <Ios/Ifstream.hpp>
#include <Ios/Osstream.hpp>
#include <Util/CoutBuf.hpp>
#include <Util/FileUtils.hpp>
#include <Util/Tokenizer.hpp>
#include <Util/CharManipulations.hpp>
#include <Statistical/Assertions.hpp>

//#include <json/json.h>
#include <memory>
//#include <rapidjson/document.h>

#define BOOST_JSON_STACK_BUFFER_SIZE 1024
#include <boost/json/src.hpp>
#include <boost/json.hpp>

//using namespace boost::json;

#include <STD/VectorCpp.hpp>

using namespace EnjoLib;
//namespace JSONLib = rapidjson;
//namespace JSONLib = Json;
namespace JSONLib = boost::json;

JsonReader::JsonReader(bool verbose): m_verbose(verbose){}
JsonReader::~JsonReader(){}

static void parseJsonOrThrow(const Str & jsonFile, bool verbose, JSONLib::Value & root)
{
    const Str & wholeJson = JsonReader::GetJson(jsonFile, verbose);
    
    Json::CharReaderBuilder builder;
    JSONCPP_STRING err;
    
    const auto rawJsonLength = static_cast<int>(wholeJson.length());
  
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    if (!reader->parse(wholeJson.c_str(), wholeJson.c_str() + rawJsonLength, &root,
                       &err)) {
        Osstream oss;
        oss << jsonFile + ": failed to parse! Error: " << err << Nl;
      Assertions::Throw(oss.str().c_str(), "parseJsonOrThrow");
    }
}


/*
static void parseJsonOrThrow(const Str & jsonFile, bool verbose, Json::Value & root)
{
    const Str & wholeJson = JsonReader::GetJson(jsonFile, verbose);
    
    Json::CharReaderBuilder builder;
    JSONCPP_STRING err;
    
    const auto rawJsonLength = static_cast<int>(wholeJson.length());
  
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    if (!reader->parse(wholeJson.c_str(), wholeJson.c_str() + rawJsonLength, &root,
                       &err)) {
        Osstream oss;
        oss << jsonFile + ": failed to parse! Error: " << err << Nl;
      Assertions::Throw(oss.str().c_str(), "parseJsonOrThrow");
    }
}
*/

static const JSONLib::Value & GetArrayJson(const EnjoLib::Str & name, bool verbose)
{
    const Str jsonFile = name + ".json";
    //rapidjson::Document d;
    JSONLib::Value d;
    if (verbose)
    {LOGL << "Reading array: " << name << Nl;}
    parseJsonOrThrow(jsonFile, verbose, d);
    const JSONLib::Value& array_json = d[name.c_str()];
    //{LOGL << "Reading array: " << name << " succeeded." << Nl;}
    return array_json;
}

/*
static void parseJsonOrThrow(const Str & jsonFile, bool verbose, rapidjson::Document & d)
{
    const Str & wholeJson = JsonReader::GetJson(jsonFile, verbose);
    if (d.Parse(wholeJson.c_str()).HasParseError())
    {
        Assertions::Throw((jsonFile + " failed to parse\n").c_str(), "parseJsonOrThrow");
    }
}

static const JSONLib::Value & GetArrayJson(const EnjoLib::Str & name, bool verbose)
{
    const Str jsonFile = name + ".json";
    rapidjson::Document d;
    if (verbose)
    {LOGL << "Reading array: " << name << Nl;}
    parseJsonOrThrow(jsonFile, verbose, d);
    const JSONLib::Value& array_json = d[name.c_str()];
    //{LOGL << "Reading array: " << name << " succeeded." << Nl;}
    return array_json;
}
*/
class JsonValueWrapper
{
public:
    JsonValueWrapper(const JSONLib::Value & obj, const EnjoLib::Str & identif)
    : m_obj(obj)
    , m_id(identif)
    {

    }
    const JSONLib::Value & GetValueJson(const EnjoLib::Str & name) const;


private:

    const JSONLib::Value & m_obj;
    const EnjoLib::Str m_id;
};

const JSONLib::Value & JsonValueWrapper::GetValueJson(const EnjoLib::Str & name) const
{
    //{LOGL << "Reading variable: " << name << " of " << m_id << Nl;}
    //if (not m_obj.HasMember(name.c_str()))
    if (not m_obj.isMember(name.c_str()))
    {
        const EnjoLib::Str msg = "No member " + name + " in " + m_id;
        Assertions::Throw(msg.c_str(), "GetValue");
    }
    return m_obj[name.c_str()];
}

EnjoLib::Array<BatteryParams> JsonReader::ReadBatteries(bool verbose) const
{
    const CharManipulations cman;
    std::vector<BatteryParams> ret;
    const Str idd = "batteries";
    const JSONLib::Value& arr = GetArrayJson(idd, m_verbose);
    for (auto objIt = arr.begin(); objIt != arr.end(); ++objIt)
    {
        BatteryParams batObj;
        int count = 1;
        const JSONLib::Value & bat = *objIt;
        JsonValueWrapper jwrap(bat, idd);
        ELO

        batObj.DISCHARGE_RATE_C_BY =    jwrap.GetValueJson("discharge_rate_c_by").asInt();
        batObj.MAX_CHARGE_V =           jwrap.GetValueJson("max_charge_v").asDouble();
        batObj.MAX_DISCHARGE_AMP =      jwrap.GetValueJson("max_discharge_amp").asDouble();
        batObj.MAX_CAPACITY_AMPH =      jwrap.GetValueJson("max_capacity_amph").asDouble();
        batObj.MIN_LOAD_AMPH =          jwrap.GetValueJson("min_load_amph").asDouble();
        batObj.DISCHARGE_PER_HOUR_PERCENT = jwrap.GetValueJson("discharge_per_hour_percent").asDouble();
        //batObj.name = bat["name"].GetString();
        //LOG <<  << Nl;
        if (bat.isMember("count")) /// TODO: Repeated pattern
        {
            count = jwrap.GetValueJson("count").asInt();
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
    const JSONLib::Value& arr = GetArrayJson(idd, m_verbose);
    for (auto compIt = arr.begin(); compIt != arr.end(); ++compIt)
    {
        Computer compObj;
        int count = 1;
        const JSONLib::Value & comp = *compIt;
        JsonValueWrapper jwrap(comp, idd);
        ELO

        compObj.cores =         jwrap.GetValueJson("cores").asInt();
        compObj.wattPerCore =   jwrap.GetValueJson("watt_per_core").asDouble();
        compObj.hashPerCore =   jwrap.GetValueJson("hash_per_core").asDouble();
        /// TODO: hash per core scaling factor
        //compObj.scalingFactor = 0.85;

        compObj.hashPerCore =    jwrap.GetValueJson("hash_per_core").asDouble();
        compObj.wattAsleep =     jwrap.GetValueJson("watt_asleep").asDouble();
        compObj.wattIdle =       jwrap.GetValueJson("watt_idle").asDouble();
        compObj.maxTempCelcius = jwrap.GetValueJson("max_temp_celcius").asDouble();
        compObj.minRunHours =    jwrap.GetValueJson("min_run_hours").asInt();
        compObj.name =           jwrap.GetValueJson("name").asString();
        compObj.hostname =       jwrap.GetValueJson("hostname").asString();
        compObj.macAddr =        jwrap.GetValueJson("MAC").asString();
        if (comp.isMember("is_poweroff"))
        {
            compObj.isPoweroff = jwrap.GetValueJson("is_poweroff").asBool();
        }
        //LOG <<  << Nl;
        if (comp.isMember("count"))
        {
            count = jwrap.GetValueJson("count").asInt();
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
    const JSONLib::Value& arr = GetArrayJson(idd, m_verbose);
    for (auto itr = arr.begin(); itr != arr.end(); ++itr)
    {
        Habit obj;
        int count = 1;
        const JSONLib::Value & habit = *itr;
        JsonValueWrapper jwrap(habit, idd);
        ELO

        obj.name = jwrap.GetValueJson("name").asString();
        //{LOGL << obj.name << Nl;}
        obj.watt = jwrap.GetValueJson("watt").asDouble();
        if (habit.isMember("watt_asleep"))
        {
            obj.watt_asleep = jwrap.GetValueJson("watt_asleep").asDouble();
        }
        if (habit.isMember("schedule"))
        {
             obj.schedule =         jwrap.GetValueJson("schedule").asString();
             obj.duration_hours =   jwrap.GetValueJson("duration_hours").asDouble();
        }

        //LOG <<  << Nl;
        if (habit.isMember("count"))
        {
            count = jwrap.GetValueJson("count").asInt();
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
    //rapidjson::Document d;
    JSONLib::Value d;
    parseJsonOrThrow(jsonFile, m_verbose, d);
    JsonValueWrapper jwrap(d, idd);

    ret.voltage     = jwrap.GetValueJson("voltage").asInt();
    ret.generating  = jwrap.GetValueJson("generate").asBool();
    ret.buying      = jwrap.GetValueJson("buy").asBool();
    ret.selling     = jwrap.GetValueJson("sell").asBool();

    return ret;
}

ConfigSol JsonReader::ReadConfigSol(bool verbose) const
{
    ConfigSol ret;
    const Str jsonFile = "config-volatile.json";
    const Str idd = jsonFile;
    //rapidjson::Document d;
    JSONLib::Value d;
    parseJsonOrThrow(jsonFile, m_verbose, d);
    JsonValueWrapper jwrap(d, idd);
    
    /// TODO: This should be secured better, like the rest, but it's multilayered.
    ret.m_outDir     = d["paths"]["DIR_TMP"].asString();
    return ret;
}

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

EnjoLib::Str JsonReader::GetJson(const EnjoLib::Str & fileName, bool verbose)
{
    const char *homedir = nullptr;

    if ((homedir = getenv("HOME")) == NULL) {
        homedir = getpwuid(getuid())->pw_dir;
    }
    if (homedir == nullptr)
    {
        Assertions::Throw("Not found home dir", "JsonReader::GetJson");
    }
    const Str path = Str(homedir) + "/.config/solar/" + fileName;
    if (verbose)
    {LOGL << "Reading json: " << path << Nl;}
    Ifstream fcomps(path);
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
    const JSONLib::Value& s = d["stars"];
    LOGL << s.asInt() << Nl;

    }
    */
