#include "OptimizerEnProfit.h"

#include "OptiEnProfitDataModel.h"
#include "OptiEnProfitSubject.h"
#include "TimeUtil.h"

#include <Ios/Osstream.hpp>
#include <Ios/Ofstream.hpp>
#include <Statistical/Distrib.hpp>
#include <Math/GeneralMath.hpp>
#include <Util/CoutBuf.hpp>
#include <Util/CharManipulations.hpp>

using namespace std;
using namespace EnjoLib;

/// TODO: UTest & refactor
void OptimizerEnProfit::PrintSolution(const EnjoLib::Matrix & bestMat) const
{
    const int currHour = TimeUtil().GetCurrentHour();
    OptiSubjectEnProfit osub(m_dataModel);
    osub.GetVerbose(bestMat, true);
    const CharManipulations cman;
    for (int i = 0; i < bestMat.size(); ++i)
    {
        //GnuplotPlotTerminal1d(bestMat.at(i), "Best solution = " + CharManipulations().ToStr(m_goal), 1, 0.5);
    }
    const Distrib distr;
    const DistribData & distribDat = distr.GetDistrib(m_goals);
    if (distribDat.IsValid())
    {
        //GnuplotPlotTerminal2d(distribDat.data, "Solution distribution", 1, 0.5);
    }
    Str cmds = "";
    ELO
    LOG << "\nComputer start schedule:\n";
    Osstream oss;
    for (int i = 0; i < bestMat.size(); ++i)
    {
        const Computer & comp = m_dataModel.GetComputers().at(i);
        const VecD & best = bestMat.at(i);
        LOG << comp.name << Nl;
        LOG << cman.Replace(best.Print(), " ", "") << Nl;
        
        const Str cmdsSSHbare = "ssh -o ConnectTimeout=35 -n " + comp.hostname + " ";
        const Str cmdsSSH = cmdsSSHbare + " 'hostname; echo \"";
        const Str cmdWOL = "wakeonlan " + comp.macAddr;
        const Str cmdSuspendAt = "systemctl suspend\"           | at ";
        const Str cmdMinuteSuffix = ":00'\n";
        
        bool onAtFirstHour = false;
        int lastHourOn = -1;
        int lastDayOn = -1;
        const int horizonHours = m_dataModel.GetHorizonHours();
        for (int i = 1; i < horizonHours; ++i)
        {
            const int ihour = i + currHour;
            const int hour = ihour % HOURS_IN_DAY;
            const int day  = GMat().round(ihour / static_cast<double>(HOURS_IN_DAY));
            const int dayPrev  = GMat().round((ihour-1) / static_cast<double>(HOURS_IN_DAY));
            if (day != dayPrev)
            {
                //LOG << Nl;
            }

            const bool onPrev = best.at(i-1);
            const bool onCurr = best.at(i);
            if (not onPrev && onCurr)
            {
                lastHourOn = hour;
                lastDayOn = day;
            }
            const int hourPrev = (ihour - 1) % HOURS_IN_DAY;
            if (lastHourOn > 0)
            {
                if (not onCurr) // Switch off
                {
                    LOG << "day " << lastDayOn << ", hour " << lastHourOn << "-" << hourPrev<< Nl;                  
                    if (lastDayOn == 1)
                    {
                        // Wake up
                        oss << "echo \"" << cmdWOL << "\" | at " << lastHourOn << cmdMinuteSuffix;
                        // Put to sleep
                        oss << cmdsSSH << cmdSuspendAt << hourPrev << cmdMinuteSuffix;
                    }
                    
                    lastHourOn = -1;
                }
                else if (i == horizonHours - 1)
                {
                    LOG << "day " << lastDayOn << ", hour " << lastHourOn << "-.." << Nl;
                }
            }
            if (onCurr && i == 1)
            {
                // Start now, right at the beginning! Battery probably already too overloaded
                oss << cmdWOL << "\n";
                onAtFirstHour = true;
            }
            else
            if (onPrev && not onCurr && day == 1)
            {
                if (onAtFirstHour) // Was started at the beginning already. Be sure to suspend later on.
                {
                    LOG << "day 1, hour !-" << hourPrev << Nl;
                    oss << cmdsSSH << cmdSuspendAt << hourPrev << cmdMinuteSuffix;
                }
            }
        }
        LOG << Nl;
    }
    
    LOG << "Commands:\n\n";
    LOG << oss.str();
    
    const Str fileCmds = "/tmp/cmds.sh";
    Ofstream ofs(fileCmds);
    ofs << oss.str();
    
    LOG << "\nSaved commands:\nbash " << fileCmds << Nl;
}

