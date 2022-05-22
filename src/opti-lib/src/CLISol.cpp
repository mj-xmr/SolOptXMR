#include "CLISol.h"
#include "CLIResultSol.h"
#include "ConfigSol.h"

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
    ConfigSol confSol;

    //const char * OPT_PLUGIN  = "plugin";

    const char * OPT_DAYS_HORIZON = "horizon-days";
    const char * OPT_DAYS_START  = "start-day";
    const char * OPT_BATTERY_CHARGE  = "battery-charge";
    const char * OPT_SYSTEM_TYPE  = "system-type";
    const char * OPT_SYSTEM_VOLTAGE  = "system-voltage";

    EnjoLib::ProgramOptionsState popState;
    ////popState.AddStr(OPT_PLUGIN,    "Plugin name");
    popState.AddInt(OPT_DAYS_HORIZON,       ConfigSol::DESCR_DAYS_HORIZON);
    popState.AddInt(OPT_DAYS_START,         ConfigSol::DESCR_DAYS_START);
    popState.AddFloat(OPT_BATTERY_CHARGE,   ConfigSol::DESCR_BATTERY_CHARGE);
    //popState.AddStr(OPT_SYSTEM_TYPE,        ConfigSol::DESCR_SYSTEM_TYPE);
    //popState.AddInt(OPT_SYSTEM_VOLTAGE,     ConfigSol::DESCR_SYSTEM_VOLTAGE);

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

    confSol.DAYS_HORIZON    = pops.GetIntFromMap(OPT_DAYS_HORIZON);
    confSol.DAYS_START 	    = pops.GetIntFromMap(OPT_DAYS_START);
    confSol.BATTERY_CHARGE  = pops.GetFloatFromMap(OPT_BATTERY_CHARGE);
    //confSol.SYSTEM_TYPE     = pops.GetStrFromMap(OPT_SYSTEM_TYPE);
    //confSol.SYSTEM_VOLTAGE  = pops.GetIntFromMap(OPT_SYSTEM_VOLTAGE);
    //confSym.period     		    = pops.GetStrFromMap(OPT_PERIOD);
    //auto pluginName = pops.GetStrFromMap (OPT_PLUGIN);

    return EnjoLib::Result<CLIResultSol>(CLIResultSol(confSol), true);
}
