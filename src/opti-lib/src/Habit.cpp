#include "Habit.h"
#include "HabitCron.h"

#include <Statistical/Assertions.hpp>

Habit::Habit(){}
Habit::~Habit(){}

EnjoLib::Str Habit::Print() const
{
    return name;
}

void Habit::ParseSchedule()
{
    m_hoursOn = HabitCron().GetNextHoursOn(*this);
    szz = m_hoursOn.size();
}

bool Habit::IsOn(int hour) const
{
    if (not HasHour(hour))
    {
        EnjoLib::Assertions::Throw("Size above", "Habit::IsOn");
        //return false;
    }
    return m_hoursOn[hour] != 0;
    //return m_hoursOn.at(hour) != 0;
}
