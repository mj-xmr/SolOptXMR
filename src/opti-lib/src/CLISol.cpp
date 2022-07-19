#include "CLISol.h"
#include "CLIResultSol.h"
#include "ConfigSol.h"
#include "JsonReader.h"

#include <EnjoLibBoost/ProgramOptions.hpp>

#include <Util/CoutBuf.hpp>
#include <Ios/Osstream.hpp>
#include <Template/Array.hpp>

using namespace EnjoLib;

CLISol::CLISol(){}
CLISol::~CLISol(){}

EnjoLib::Str CLISol::GetAdditionalHelp() const
{
    Osstream oss;
    oss << "Additional help stub\n";
    //for (const Str & name : types.GetNames())
    {
        //oss << name << " ";
    }

    return oss.str();
}


/// TODO: Fill all the possible options
EnjoLib::Result<CLIResultSol> CLISol::GetConfigs(int argc, char ** argv) const
{
    ConfigSol confSol = JsonReader().ReadConfigSol();

    //const char * OPT_PLUGIN  = "plugin";

    const char * OPT_DAYS_HORIZON = "horizon-days";
    const char * OPT_DAYS_START  = "start-day";
    const char * OPT_RANDOM_SEED  = "random-seed";
    const char * OPT_BATTERY_CHARGE  = "battery-charge";
    const char * OPT_BATTERY_CHARGE_MAX_PERCENTAGE  = "battery-charge-max-percent";
    const char * OPT_SYSTEM_TYPE  = "system-type";
    const char * OPT_SYSTEM_VOLTAGE  = "system-voltage";
    const char * OPT_HASHRATE_BONUS  = "hashrate-bonus";
    const char * OPT_OUT_DIR   = "out";

    EnjoLib::ProgramOptionsState popState;
    ////popState.AddStr(OPT_PLUGIN,    "Plugin name");
    popState.AddInt(OPT_DAYS_HORIZON,       ConfigSol::DESCR_DAYS_HORIZON);
    popState.AddInt(OPT_DAYS_START,         ConfigSol::DESCR_DAYS_START);
    popState.AddInt(OPT_RANDOM_SEED,         ConfigSol::DESCR_RANDOM_SEED);
    popState.AddFloat(OPT_BATTERY_CHARGE,   ConfigSol::DESCR_BATTERY_CHARGE);
    popState.AddFloat(OPT_BATTERY_CHARGE_MAX_PERCENTAGE,   ConfigSol::DESCR_BATTERY_CHARGE_MAX_PERCENTAGE);
    popState.AddFloat(OPT_HASHRATE_BONUS,   ConfigSol::DESCR_HASHRATE_BONUS);
    //popState.AddStr(OPT_SYSTEM_TYPE,        ConfigSol::DESCR_SYSTEM_TYPE);
    //popState.AddInt(OPT_SYSTEM_VOLTAGE,     ConfigSol::DESCR_SYSTEM_VOLTAGE);
    popState.AddStr(OPT_OUT_DIR,    "Output directory");

    popState.ReadArgs(argc, argv);
    const EnjoLib::ProgramOptions pops(popState);

    if (pops.IsHelpRequested())
    {
        /// TODO: Try also to react also on -h by adding here:
        /**
        bool ProgramOptions::IsHelpRequested() const
        {
            return GetBoolFromMap(ProgramOptionsState::OPT_HELP);
        }
        return GetBoolFromMap(ProgramOptionsState::OPT_HELP || return GetBoolFromMap(ProgramOptionsState::OPT_HELP_MIN););
        const char * OPT_HELP_MIN = "h";
        */
        ELO
        LOG << pops.GetHelp() << Nl;

        LOG << GetAdditionalHelp();
        LOG << Nl;
        //LOG << "The default path of the script is: " << ConfigTS::DEFAULT_SCRIPT_FILE_NAME << Nl;
        LOG << "\n(C) 2022-2023 mj-xmr. Licensed under AGPLv3.\n";
        LOG << "https://github.com/mj-xmr/SolOptXMR\n";
        LOG << "\nRemember to be happy.\n\n";
        return EnjoLib::Result<CLIResultSol>(CLIResultSol(confSol), false);
    }

    //confSym.dates.Set0();

//#ifdef NO_RANDOM
    //confSol.RANDOM_SEED = 1; // An idea for later perhaps.
//#endif // NO_RANDOM

    confSol.DAYS_HORIZON    = pops.GetIntFromMap(OPT_DAYS_HORIZON);
    confSol.DAYS_START 	    = pops.GetIntFromMap(OPT_DAYS_START);
    confSol.RANDOM_SEED     = pops.GetIntFromMap(OPT_RANDOM_SEED);
    confSol.HASHRATE_BONUS  = pops.GetFloatFromMap(OPT_HASHRATE_BONUS);
    confSol.BATTERY_CHARGE  = pops.GetFloatFromMap(OPT_BATTERY_CHARGE);
    confSol.BATTERY_CHARGE_MAX_PERCENTAGE = pops.GetFloatFromMap(OPT_BATTERY_CHARGE_MAX_PERCENTAGE);
    confSol.m_outDir        = pops.GetStrFromMap(OPT_OUT_DIR, confSol.m_outDir);
    //confSol.SYSTEM_TYPE     = pops.GetStrFromMap(OPT_SYSTEM_TYPE);
    //confSol.SYSTEM_VOLTAGE  = pops.GetIntFromMap(OPT_SYSTEM_VOLTAGE);
    //confSym.period     		    = pops.GetStrFromMap(OPT_PERIOD);
    //auto pluginName = pops.GetStrFromMap (OPT_PLUGIN);
    return EnjoLib::Result<CLIResultSol>(CLIResultSol(confSol), true);
}
