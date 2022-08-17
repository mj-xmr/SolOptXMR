//#include "OptimizerEnProfit.h" /// TODO: Use a smaller header after the extraction
#include "OptiEnProfitResults.h"

//#include "ConfigSol.h"
#include "JsonReader.h"

#include <Visual/Ascii.hpp>
#include <Ios/Osstream.hpp>
#include <Util/CoutBuf.hpp>
#include <Util/Tokenizer.hpp>
#include <Util/CharManipulations.hpp>

#include <UnitTest++/UnitTest++.h>
//#include <initializer_list>

using namespace EnjoLib;

static Computer GetCompTestSched()
{
    Computer compTest;
    
    return compTest;
}

static Str GetSched2ExpPlot(const VecD & schedule)
{
    Str ret;// = '\n';
    for (FP val : schedule)
    {
        ret += AsciiPlotGuts().GetPercentToAscii(val, 0, 1, true, false, false);
    }
    //ret += '\n';
    return ret;
}

static Str GetStartHourToSchedule(int startHour, int endHour = -1)
{
    Osstream oss;
    
    oss << "day 0, hour " << startHour << "-";
    if (endHour >= 0)
    {
        oss << endHour;
    }
    else
    {
        oss << "..";
    }
    return oss.str();
}

TEST(CompSched_open_ended)
{
    const CharManipulations cman;
    const Computer & comp0 = GetCompTestSched();
    const VecD schedule = {0, 0, 0, 1, 1, 1, 1, 1, 1};
    const int currHour = 0;
    const int startHour = 3;
    const Str & schedStr = OptiEnProfitResults().PrintScheduleComp(comp0, schedule, currHour);
    CHECK(schedStr.size());
    const Tokenizer tok;
    const VecStr & toks = tok.Tokenize(schedStr, '\n');
    LOGL << schedStr << Nl;
    CHECK(toks.size() > 1);
    
    const Str & exp = GetSched2ExpPlot(schedule);
    CHECK_EQUAL(exp, toks.at(1));
    
    const Str & expText = GetStartHourToSchedule(startHour);
    CHECK_EQUAL(expText, toks.at(2));
}

TEST(CompSched_finalized)
{
    const Computer & comp0 = GetCompTestSched();
    const VecD schedule = {0, 0, 0, 1, 1, 1, 1, 1, 0};
    const int currHour = 0;
    const int startHour = 3;
    const int endHour = 7;
    const Str & schedStr = OptiEnProfitResults().PrintScheduleComp(comp0, schedule, currHour);
    const Tokenizer tok;
    const VecStr & toks = tok.Tokenize(schedStr, '\n');
    LOGL << schedStr << Nl;
    const Str & exp = GetSched2ExpPlot(schedule);
    CHECK_EQUAL(exp, toks.at(1));
    const Str & expText = GetStartHourToSchedule(startHour, endHour);
    CHECK_EQUAL(expText, toks.at(2));
}
