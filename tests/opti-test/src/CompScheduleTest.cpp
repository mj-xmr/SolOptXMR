//#include "OptimizerEnProfit.h" /// TODO: Use a smaller header after the extraction
#include "OptiEnProfitResults.h"

#include "ConfigSol.h"
#include "Computer.h"
#include "OptiTestUtil.h"

#include <Visual/Ascii.hpp>
#include <Ios/Osstream.hpp>
#include <Util/CoutBuf.hpp>
#include <Util/Tokenizer.hpp>
#include <Util/CharManipulations.hpp>
#include <Math/GeneralMath.hpp>

#include <UnitTest++/UnitTest++.h>
//#include <initializer_list>

using namespace EnjoLib;

static int SCHEDULE_CURR_HOUR = 0;

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
    const CharManipulations cman;
    oss << "day 0, hour " << (startHour > 0 ? cman.ToStr(startHour) : "!") << "-";
    if (endHour >= 0)
    {
        oss << endHour % 24;
    }
    else
    {
        oss << "..";
    }
    return oss.str();
}

static Str GetStartHourToWakeup(int startHour, const Computer & comp)
{
    Osstream oss;
    if (startHour > SCHEDULE_CURR_HOUR)
    {
        oss << "echo \"";
    }
    oss << "wakeonlan " << OptiTestUtil::compSched_macAddr;
    if (startHour > SCHEDULE_CURR_HOUR)
    {
        /// TODO: All "at"s should also include the day when to start, not just today, unless we assume starting once only every day, which makes sense as well, but make it a parameter or sth.
        oss << "\" | at " << startHour << ":00";
    }
    return oss.str();
}

static Str GetStartHourToSleep(const Computer & comp, int endHour)
{
    if (endHour < 0)
    {
        return "";
    }
    Osstream oss;
    oss
    << "echo \"" << "ssh -o ConnectTimeout=" << OptiEnProfitResults::SSH_TIMEOUT_S
    << " -n " << OptiTestUtil::compSched_hostname
    << " 'hostname; systemctl --no-wall ";
    if (comp.isPoweroff)
    {
        oss << "poweroff";
    }
    else
    {
        oss << "suspend";
    }
    oss
    << "'\" | at ";
    if (endHour >= 24)
    {
        oss << endHour % 24 << ":00" << " + " << GMat().Floor(endHour / 24.0) << " days";
    }
    else
    {
        oss << endHour << ":00";
    }
    return oss.str();
}

//ssh -o ConnectTimeout=35 -n Miner_2049er  'hostname; echo "systemctl suspend" | at 7:00

static void CompSchedTestGraph(const VecD & schedule)
{
    const OptiEnProfitResults proRes;
    const Computer & comp0 = OptiTestUtil().GetCompTestSched();

    const Str & schedStr = proRes.PrintScheduleCompGraph(comp0, schedule);
    CHECK(schedStr.size());
    const Tokenizer tok;
    const VecStr & toks = tok.Tokenize(schedStr, '\n');
    LOGL << schedStr << Nl;
    CHECK(toks.size() > 1);

    const Str & exp = GetSched2ExpPlot(schedule);
    CHECK_EQUAL(exp, toks.at(1));
}

static VecStr CompSchedTestCommands(const VecD & schedule, int currHour, int startHour, int endHour, const Computer & comp0 = OptiTestUtil().GetCompTestSched())
{
    ELO
    const OptiEnProfitResults proRes;
    const Tokenizer tok;
    const ConfigSol conf;
    const OptiEnProfitResults::CommandsInfos & cmdInfo = proRes.PrintCommandsComp(conf, comp0, schedule, currHour);
    LOG << "Info: " << cmdInfo.infos;
    LOG << "Cmds: " << cmdInfo.commands;
    const VecStr & toksInfo = tok.Tokenize(cmdInfo.infos, '\n');
    const Str & expTextInfo = GetStartHourToSchedule(startHour, endHour);
    CHECK(toksInfo.size());
    CHECK_EQUAL(expTextInfo, toksInfo.at(0));
    const VecStr & toksCmds = tok.Tokenize(cmdInfo.commands, '\n');

    const Str & expTextCmdWakeUp = GetStartHourToWakeup(startHour, comp0);
    const Str & expTextCmdSleep  = GetStartHourToSleep(comp0, endHour);

    CHECK_EQUAL(expTextCmdWakeUp, toksCmds.at(1));
    LOG << "Was & exp:\n" << toksCmds.at(2) << Nl << expTextCmdSleep << Nl;
    CHECK_EQUAL(expTextCmdSleep,  toksCmds.at(2));

    return toksCmds;
}

TEST(CompSched_open_ended)
{
    const VecD schedule = {0, 0, 0, 1, 1, 1, 1, 1, 1};

    CompSchedTestGraph(schedule);
    {
        const int currHour = SCHEDULE_CURR_HOUR;
        const int startHour = 3;
        const int endHour = -1;

        const VecStr & toksCmds = CompSchedTestCommands(schedule, currHour, startHour, endHour);
        CHECK(toksCmds.size() >= 1);

    } //echo "wakeonlan DE:AD:BE:EF:AA:BB" | at 3:00
}

TEST(CompSched_finalized)
{
    const VecD schedule = {0, 0, 0, 1, 1, 1, 1, 1, 0};

    CompSchedTestGraph(schedule);
    {
        ELO
        const int currHour = SCHEDULE_CURR_HOUR;
        const int startHour = 3;
        const int endHour = 7;

        const VecStr & toksCmds = CompSchedTestCommands(schedule, currHour, startHour, endHour);
        CHECK(toksCmds.size() >= 2);
    }
}


TEST(CompSched_start_immediately)
{
    const VecD schedule = {1, 1, 1, 1, 1, 1, 1, 1, 0};

    CompSchedTestGraph(schedule);

    {
        ELO
        const int currHour = SCHEDULE_CURR_HOUR;
        const int startHour = 0;
        const int endHour = 7;

        const VecStr & toksCmds = CompSchedTestCommands(schedule, currHour, startHour, endHour);
        CHECK(toksCmds.size() >= 2);
    }
}


TEST(CompSched_start_immediately_restart_till_end)
{
    const VecD schedule = {1, 1, 1, 1, 1, 0, 1, 1, 1};

    CompSchedTestGraph(schedule);

    {
        ELO
        const int currHour = SCHEDULE_CURR_HOUR;
        const int startHour = 0;
        const int endHour = 4;

        const VecStr & toksCmds = CompSchedTestCommands(schedule, currHour, startHour, endHour);
        CHECK(toksCmds.size() >= 2);
    }
}


TEST(CompSched_start_immediately_restart)
{
    const VecD schedule = {1, 1, 1, 1, 1, 0, 1, 1, 0};

    CompSchedTestGraph(schedule);

    {
        ELO
        const int currHour = SCHEDULE_CURR_HOUR;
        const int startHour = 0;
        const int endHour = 4;

        const VecStr & toksCmds = CompSchedTestCommands(schedule, currHour, startHour, endHour);
        CHECK(toksCmds.size() >= 2);
        //CHECK_EQUAL(5, toksCmds.size()); /// TODO
    }
}

TEST(CompSched_more_than_2_days)
{
    const int maxHour = 48;
    VecD schedule(maxHour);
    
    {
        ELO
        const int currHour = SCHEDULE_CURR_HOUR;
        const int startHour = 21;
        const int endHour = 28;

        for (int i = startHour; i <= endHour && i < maxHour; ++i)
        {
            schedule.at(i) = 1;
        }
        CompSchedTestGraph(schedule);

    
        const VecStr & toksCmds = CompSchedTestCommands(schedule, currHour, startHour, endHour);
        CHECK(toksCmds.size() >= 2);
        //CHECK_EQUAL(5, toksCmds.size()); /// TODO
    }
}


TEST(CompSched_poweroff)
{
    const VecD schedule = {0, 0, 0, 1, 1, 1, 1, 1, 0};

    CompSchedTestGraph(schedule);

    {
        ELO
        const int currHour = SCHEDULE_CURR_HOUR;
        const int startHour = 3;
        const int endHour = 7;
        Computer comp0 = OptiTestUtil().GetCompTestSched();
        comp0.isPoweroff = true;
        
        const VecStr & toksCmds = CompSchedTestCommands(schedule, currHour, startHour, endHour, comp0);
        CHECK(toksCmds.size() >= 2);
    }
}
//const Computer & comp0 = OptiTestUtil().GetCompTestSched())

/// TODO: Quite a corner case:
/*
TEST(CompSched_start_immediately_dont_finish)
{
    const VecD schedule = {1, 1, 1, 1, 1, 1, 1, 1, 1}; // (aka - start and never finish)

    CompSchedTestGraph(schedule);

    {
        ELO
        const OptiEnProfitResults proRes;
        const Tokenizer tok;
        const Computer & comp0 = GetCompTestSched();

        const int currHour = SCHEDULE_CURR_HOUR;
        const int startHour = 0;
        const int endHour = -1;

        const VecStr & toksCmds = CompSchedTestCommands(schedule, currHour, startHour, endHour);
        const Str & expTextCmdWakeUp = GetStartHourToWakeup(startHour, comp0);
        const Str & expTextCmdSleep  = GetStartHourToSleep(comp0, endHour);

        CHECK(toksCmds.size() >= 2);
        CHECK_EQUAL(expTextCmdWakeUp, toksCmds.at(0));
        LOG << "Was & exp:\n" << toksCmds.at(1) << Nl << expTextCmdSleep << Nl;
        CHECK_EQUAL(expTextCmdSleep,  toksCmds.at(1));
    }
}
*/

