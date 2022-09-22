#include "HabitCron.h"
#include "Habit.h"
#include "TimeUtil.h"
#include "HabitCron3rd.h"

#include <Util/CoutBuf.hpp>
#include <Math/GeneralMath.hpp>
#include <Statistical/Assertions.hpp>

HabitCron::~HabitCron(){}
HabitCron::HabitCron(bool verbose)
: m_verbose(verbose)
{}

EnjoLib::VecT<int> HabitCron::GetNextHoursOn(const Habit & hab, int horizonDays) const
{
    const int hoursInDay = 24;
    const int maxI = hoursInDay * horizonDays;
    EnjoLib::VecT<int> ret(maxI);
    EnjoLib::VecT<int> retAlwaysOn(maxI, 1);
    if (hab.schedule.empty())
    {
        return retAlwaysOn;
    }
    if (not hab.defaultUseSchedule)
    {
        return retAlwaysOn;
    }
    
    const EnjoLib::VecT<int> & hours = HabitCron3rd(m_verbose).GetNextHoursOn(hab, horizonDays);
    for (unsigned i = 1; i < hours.size(); ++i)
    {
        const int prev = hours.at(i - 1);
        const int curr = hours.at(i);

        const int diff = curr - prev;
        EnjoLib::Assertions::IsTrue(diff >= hab.duration_hours, ("difference between events is < than duration for " + hab.name).c_str());
    }
    for (int i = 0; i < ret.size(); ++i)
    {
        if (hours.Contains(i))
        {
            // Propagate the act of switching on across the duration
            for (int j = i; j < ret.size() && j < i + hab.duration_hours; ++j)
            {
                ret.atw(j) = 1;
            }
        }
    }
    //catch (cron::bad_cronexpr const & ex)
    {
       //LOGL  << ex.what() << '\n';
    }
    return ret;
}

