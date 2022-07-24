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
        
        Str cmdsComp = "ssh -n " + comp.hostname + " 'hostname; echo \"";
        

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
            if (lastHourOn > 0)
            {
                if (not onCurr) // Switch off
                {
                    const int hourPrev = (ihour - 1) % HOURS_IN_DAY;
                    LOG << "day " << lastDayOn << ", hour " << lastHourOn << "-" << hourPrev<< Nl;                  
                    if (lastDayOn == 1)
                    {
                        // Wake up
                        oss << cmdsComp << "wakeonlan " << comp.macAddr << "\" | at " << lastHourOn <<   ":00'\n";
                        // Put to sleep
                        oss << cmdsComp << "systemctl suspend\"           | at " << hourPrev <<     ":00'\n";
                    }
                    
                    lastHourOn = -1;
                }
                else if (i == horizonHours - 1)
                {
                    LOG << "day " << lastDayOn << ", hour " << lastHourOn << "-.." << Nl;
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

