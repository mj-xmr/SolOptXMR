#include "Habit.h"
#include "HabitCron.h"

#include <Statistical/Assertions.hpp>

Habit::Habit(){}
Habit::~Habit(){}

EnjoLib::Str Habit::Print() const
{
    return name;
}

void Habit::ParseSchedule() /// TODO: Figure out a way to parse it only once
{
    m_hoursOn = HabitCron().GetNextHoursOn(*this);
    szz = m_hoursOn.size();
}

bool Habit::IsOn(int hour) const
{
    if (hour > szz)
    {
        EnjoLib::Assertions::Throw("Size above", "Habit::IsOn");
        //return false;
    }
    return m_hoursOn[hour] != 0;
    //return m_hoursOn.at(hour) != 0;
}
