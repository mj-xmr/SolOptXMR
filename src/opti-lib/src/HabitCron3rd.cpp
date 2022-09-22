#include "HabitCron3rd.h"
#include "Habit.h"

#include <Util/CoutBuf.hpp>
#include <Math/GeneralMath.hpp>
#include <Statistical/Assertions.hpp>

#include "croncpp.h"

HabitCron3rd::~HabitCron3rd(){}
HabitCron3rd::HabitCron3rd(bool verbose)
: m_verbose(verbose)
{}

EnjoLib::VecT<int> HabitCron3rd::GetNextHoursOn(const Habit & hab, int horizonDays) const
{
    const int hoursInDay = 24;
    /// TODO: This assumes, that if anything started at least an hour before, and lasted more than that hour, it won't be registered here.
    /// Solution: start asking cron 24 hours before, and finally cut the first 24 hours. Unit tests needed for this.
    const std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    const std::string str = hab.schedule.str();
    
    EnjoLib::VecT<int> hours;
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
                LOGL << "HabitCron3rd:: " << hab.name << ": " << diffDays << ", " << diffHours << EnjoLib::Nl;
            }
            next = cron::cron_next(cron, next);
        }
    }
    return hours;
}

