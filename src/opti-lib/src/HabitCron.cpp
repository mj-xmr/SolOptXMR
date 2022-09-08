#include "HabitCron.h"
#include "Habit.h"
#include "TimeUtil.h"

#include <Template/Array.hpp>
#include <Util/CoutBuf.hpp>
#include <Math/GeneralMath.hpp>
#include <Statistical/Assertions.hpp>

#include "croncpp.h"

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
    /// TODO: This assumes, that if anything started at least an hour before, and lasted more than that hour, it won't be registered here.
    /// Solution: start asking cron 24 hours before, and finally cut the first 24 hours. Unit tests needed for this.
    const std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    const std::string str = hab.schedule.str();
    
    EnjoLib::VecT<int> hours;
    // Trying to isolate the cron library.
    const auto cron = cron::make_cron(str);
    //try
    {

        std::time_t next = cron::cron_next(cron, now);
        std::time_t prev = {};
        int iter = 0;
        const int maxIter = 100;
        while(next != prev && next > prev && iter++ < maxIter)
        {
            const double diffSeconds = std::difftime(next, now);
            const double diffHours = EnjoLib::GMat().round(diffSeconds / 3600.0);
            const double diffDays = EnjoLib::GMat().round(diffHours / static_cast<double>(hoursInDay));
            if (diffDays > horizonDays)
            {
                break;
            }
            hours.push_back(diffHours);
            prev = next;
            if (m_verbose) {
                LOGL << "HabitCron:: " << hab.name << ": " << diffDays << ", " << diffHours << EnjoLib::Nl;
            }
            next = cron::cron_next(cron, next);
        }
    }
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

