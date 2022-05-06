#include "ConfigSol.h"

#include "ConfigDirs.h"
#include "Consts.h"

//#include <Util/Except.hpp>
#include <Ios/Osstream.hpp>
#include <Util/CharManipulations.hpp>
#include <Util/Pair.hpp>

using namespace std;
using namespace EnjoLib;

ConfigSol::~ConfigSol(){}
ConfigSol::ConfigSol()
{
}

EnjoLib::Str ConfigSol::GetFileNameProt() const
{
    return "configSol.txt";
}

void ConfigSol::FromOptsNumeric(const OptionsNumeric & optsNum)
{
}

void ConfigSol::RegisterAndReadBools(EnjoLib::Istream & f)
{
}

void ConfigSol::RegisterAndReadInts(EnjoLib::Istream & f)
{
    RegisterAndReadInt (f, DAYS_HORIZON, 3);
    RegisterAndReadInt (f, DAYS_START, 0);
}
void ConfigSol::RegisterAndReadFloats(EnjoLib::Istream & f)
{
}
void ConfigSol::RegisterAndReadStrs(EnjoLib::Istream & f)
{
}
void ConfigSol::UpdateFromOther(const ConfigSol & cfgTSCmdLine)
{
    /*
    if (cfgTSCmdLine.PLOT_LAGS_NUM > 0)
    {
        PLOT_LAGS_NUM = cfgTSCmdLine.PLOT_LAGS_NUM;
    }
    if (cfgTSCmdLine.PLOT_PERIOD_NUM > 0)
    {
        PLOT_PERIOD_NUM = cfgTSCmdLine.PLOT_PERIOD_NUM;
    }
    */
}
