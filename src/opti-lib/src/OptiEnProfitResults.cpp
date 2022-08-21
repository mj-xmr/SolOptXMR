#include "OptiEnProfitResults.h"
#include "OptimizerEnProfit.h"

#include "OptiEnProfitDataModel.h"
#include "OptiEnProfitSubject.h"
#include "TimeUtil.h"
#include "SolUtil.h"

#include <Ios/Osstream.hpp>
#include <Ios/Ofstream.hpp>
#include <Statistical/Distrib.hpp>
#include <Math/GeneralMath.hpp>
#include <Util/CoutBuf.hpp>
#include <Util/ToolsMixed.hpp>
#include <Util/CharManipulations.hpp>
#include <Visual/AsciiPlot.hpp>

using namespace std;
using namespace EnjoLib;

const int OptiEnProfitResults::SSH_TIMEOUT = 60;

/// TODO: UTest & refactor
void OptimizerEnProfit::PrintSolution(const EnjoLib::Matrix & bestMat) const
{
    ELO
    OptiSubjectEnProfit osub(m_dataModel);
    osub.GetVerbose(bestMat, true);
    const CharManipulations cman;
    const SolUtil sot;
    for (int i = 0; i < bestMat.size(); ++i)
    {
        //GnuplotPlotTerminal1d(bestMat.at(i), "Best solution = " + CharManipulations().ToStr(m_goal), 1, 0.5);
    }
    const Distrib distr;
    const DistribData & distribDat = distr.GetDistrib(m_goals);
    if (distribDat.IsValid())
    {
        const Str & dstr = distr.PlotLine(distribDat, false, true, true);
        LOG << Nl << sot.GetT() <<  "Distribution of solutions:" << Nl<< dstr << Nl;
        //GnuplotPlotTerminal2d(distribDat.data, "Solution distribution", 1, 0.5);
    }
    Str cmds = "";
    
    LOG << "\nComputer start schedule:\n";
    Osstream oss;
    const int currHour = TimeUtil().GetCurrentHour();
    for (int i = 0; i < bestMat.size(); ++i)
    {
        const Computer & comp = m_dataModel.GetComputers().at(i);
        const VecD & best = bestMat.at(i);
        LOG << OptiEnProfitResults().PrintScheduleComp(comp, best);
        const OptiEnProfitResults::CommandsInfos & cmdInfo = OptiEnProfitResults().PrintCommandsComp(comp, best, currHour);
        LOG << cmdInfo.infos;
        oss << cmdInfo.commands;
    }

    LOG << "Commands:\n\n";
    LOG << oss.str();

    const Str fileCmds = "/tmp/cmds.sh";
    Ofstream ofs(fileCmds);
    ofs << oss.str();

    LOG << Nl << sot.GetT() << "Saved commands:\nbash " << fileCmds << Nl;
}

OptiEnProfitResults:: OptiEnProfitResults() {}
OptiEnProfitResults::~OptiEnProfitResults() {}

EnjoLib::Str OptiEnProfitResults::PrintScheduleComp(const Computer & comp, const VecD & best) const
{
    Osstream oss;
    oss << comp.name << Nl;
    const double maxx = 1;
    oss << AsciiPlot::Build()(AsciiPlot::Pars::MAXIMUM, maxx).Finalize().Plot(best) << Nl;
    return oss.str();
}

OptiEnProfitResults::CommandsInfos OptiEnProfitResults::PrintCommandsComp(const Computer & comp, const VecD & best, int currHour, int maxDayCmdsLimit) const
{
    CommandsInfos ret;
    Osstream ossCmd, ossInfo;
    const CharManipulations cman;
    //oss << cman.Replace(best.Print(), " ", "") << Nl;

    const Str cmdsSSHbare = "ssh -o ConnectTimeout=" + cman.ToStr(SSH_TIMEOUT) + " -n " + comp.hostname + " ";
    const Str cmdsSSH = "echo \"" + cmdsSSHbare + "'hostname; ";
    const Str cmdWOL = "wakeonlan " + comp.macAddr;
    //const Str cmdSuspendAt = "systemctl suspend\"           | at ";
    const Str cmdSuspendAt = "systemctl suspend'\" | at ";
    const Str cmdMinuteSuffix = ":00";

    bool onAtFirstHour = false;
    int lastHourOn = -1;
    int lastDayOn = -1;
    //const int horizonHours = m_dataModel.GetHorizonHours();
    const int horizonHours = best.size();
    for (int i = 1; i < horizonHours; ++i)
    {
        const int ihour = i + currHour;
        const int hour = ihour % OptimizerEnProfit::HOURS_IN_DAY;
        const int day  = GMat().round(ihour / static_cast<double>(OptimizerEnProfit::HOURS_IN_DAY));
        const int dayPrev  = GMat().round((ihour-1) / static_cast<double>(OptimizerEnProfit::HOURS_IN_DAY));
        if (day != dayPrev)
        {
            //ossInfo << Nl;
        }

        const bool onPrev = best.at(i-1);
        const bool onCurr = best.at(i);
        if (not onPrev && onCurr)
        {
            lastHourOn = hour;
            lastDayOn = day;
        }
        const bool isInDayLimit = maxDayCmdsLimit < 0 || lastDayOn <= maxDayCmdsLimit;
        const int hourPrev = (ihour - 1) % OptimizerEnProfit::HOURS_IN_DAY;
        if (lastHourOn > 0)
        {
            if (not onCurr) // Switch off
            {
                ossInfo << "day " << lastDayOn << ", hour " << lastHourOn << "-" << hourPrev << Nl;
                if (isInDayLimit)
                {
                    // Wake up
                    ossCmd << "echo \"" << cmdWOL << "\" | at " << lastHourOn << cmdMinuteSuffix << Nl;
                    // Put to sleep
                    ossCmd << cmdsSSH << cmdSuspendAt << hourPrev << cmdMinuteSuffix << Nl;
                }

                lastHourOn = -1;
            }
            else if (i == horizonHours - 1)
            {
                ossInfo << "day " << lastDayOn << ", hour " << lastHourOn << "-.." << Nl;
                if (isInDayLimit)
                {
                    // Wake up
                    ossCmd << "echo \"" << cmdWOL << "\" | at " << lastHourOn << cmdMinuteSuffix << Nl;
                }
            }
        }
        if (onCurr && i == 1)
        {
            // Wake up now, right at the beginning! The battery is probably already overloaded.
            ossCmd << cmdWOL << Nl;
            onAtFirstHour = true;
        }
        else
        if (onPrev && not onCurr && isInDayLimit)
        {
            if (onAtFirstHour) // Was started at the beginning already. Be sure to suspend later on.
            {
                ossInfo << "day 0, hour !-" << hourPrev << Nl;
                ossCmd << cmdsSSH << cmdSuspendAt << hourPrev << cmdMinuteSuffix << Nl;
            }
        }
    }
    ossCmd  << Nl;
    ossInfo << Nl;
        
    ret.commands    = ossCmd.str();
    ret.infos       = ossInfo.str();
        
    return ret;
}

